//
//                  Simu5G
//
// Authors: Giovanni Nardini, Giovanni Stea, Antonio Virdis (University of Pisa)
//
// This file is part of a software released under the license included in file
// "license.pdf". Please read LICENSE and README files before using it.
// The above files and the present reference are part of the software itself,
// and cannot be removed from it.
//

#include "apps/mec/WarningAlert/MECWarningAlertApp.h"

#include "apps/mec/DeviceApp/DeviceAppMessages/DeviceAppPacket_Types.h"
#include "apps/mec/WarningAlert/packets/WarningAlertPacket_Types.h"

#include "inet/common/TimeTag_m.h"
#include "inet/common/packet/Packet_m.h"

#include "inet/networklayer/common/L3AddressTag_m.h"
#include "inet/transportlayer/common/L4PortTag_m.h"

#include "nodes/mec/utils/httpUtils/httpUtils.h"
#include "nodes/mec/utils/httpUtils/json.hpp"
#include "nodes/mec/MECPlatform/MECServices/packets/HttpResponseMessage/HttpResponseMessage.h"

#include <fstream>
#include <cstdlib>
#include <ctime>

Define_Module(MECWarningAlertApp);

using namespace inet;
using namespace omnetpp;

MECWarningAlertApp::MECWarningAlertApp(): MecAppBase()
{
    circle = nullptr; // circle danger zone

}
MECWarningAlertApp::~MECWarningAlertApp()
{
    if(circle != nullptr)
    {
        if(getSimulation()->getSystemModule()->getCanvas()->findFigure(circle) != -1)
            getSimulation()->getSystemModule()->getCanvas()->removeFigure(circle);
        delete circle;
    }

}


void MECWarningAlertApp::initialize(int stage)
{
    MecAppBase::initialize(stage);

    // avoid multiple initializations
    if (stage!=inet::INITSTAGE_APPLICATION_LAYER)
        return;

    //retrieving parameters
    size_ = par("packetSize");

    // set Udp Socket
    ueSocket.setOutputGate(gate("socketOut"));
    localUePort = par("localUePort");
    ueSocket.bind(localUePort);

    hostSocket.setOutputGate(gate("socketOut"));
    //localhostPort = par("localHostPort");
    hostSocket.bind(4002);

    processedHostRequest = new cMessage("processedHostRequest");
    pktNum = 0;

    //testing
    EV << "MECWarningAlertApp::initialize - Mec application "<< getClassName() << " with mecAppId["<< mecAppId << "] has started!" << endl;

    // connect with the service registry
    //cMessage *msg = new cMessage("connectMp1");
    //scheduleAt(simTime() + 0, msg);

}

void MECWarningAlertApp::handleMessage(cMessage *msg)
{
//        MecAppBase::handleMessage(msg);
    if (!msg->isSelfMessage())
    {
        if(ueSocket.belongsToSocket(msg))
        {
            //TODO cut packet and send to worker
            // numOfSubResult = 0;
            // sendSubMatrix(msg);
            // if (getParentModule()->getName() == "mecHost1"){
            //     //TODO
            // }
            pktNum++;
            EV <<"receive no."<<pktNum<<" packet"<<endl;
            if(pktNum == 12){
            auto pk = check_and_cast<Packet *>(msg);
            auto matrixPk = dynamicPtrCast<const BytesChunk>(pk->peekAtFront<BytesChunk>());
            int numOfPara = matrixPk->getByteArraySize();
            double time = vim->calculateProcessingTime(mecAppId, 10*600*1200/1000000);//todo how to measure time
            time = time + (rand() % 1000 / (float)1000) * time;
            EV<< "test time:" << time;
            pac = check_and_cast<Packet *>(msg)->dup();
            scheduleAt(simTime()+time,processedHostRequest);
            // //handleUeMessage(msg);
            delete msg;
            return;
            }
        }
        else
        {
            //TODO from host 
            if (strcmp(msg->getName(), "SubMatrix") == 0){
            auto pk = check_and_cast<Packet *>(msg);
            auto matrixPk = dynamicPtrCast<const BytesChunk>(pk->peekAtFront<BytesChunk>());
            int numOfPara = matrixPk->getByteArraySize();
            //srand(time(NULL));
            double time = vim->calculateProcessingTime(mecAppId, 10*200*200*100/1000000);//todo how to measure time
            time = time + (rand() % 1000 / (float)1000) /5 * time;
            EV<< "test time:" << time;
            pac = check_and_cast<Packet *>(msg)->dup();
            scheduleAt(simTime()+time,processedHostRequest);
            //handleUeMessage(msg);
            delete msg;
            return;
            }
            if (strcmp(msg->getName(), "SubResult") == 0){
                if (collect(msg)) sendResultToUe();
            }

        }
    }
//    EV<<msg->getName()<<endl;
    if(strcmp(msg->getName(), "processedHostRequest") == 0)
    {
        handleMasterMessage(pac);
    }

    else return;
    //MecAppBase::handleMessage(msg);

}

void MECWarningAlertApp::finish(){
    MecAppBase::finish();
    EV << "MECWarningAlertApp::finish()" << endl;

    if(gate("socketOut")->isConnected()){

    }
}

void MECWarningAlertApp::sendSubMatrix(omnetpp::cMessage *msg){
    auto pk = check_and_cast<Packet *>(msg);
    ueAppAddress = pk->getTag<L3AddressInd>()->getSrcAddress();
    ueAppPort = pk->getTag<L4PortInd>()->getSrcPort();

    auto submatrix1 = inet::makeShared<BytesChunk>();
    submatrix1->setBytes(std::vector<uint8_t>(20000,1));
    auto submatrix2 = inet::makeShared<BytesChunk>();
    submatrix2->setBytes(std::vector<uint8_t>(20000,2));
    //matrix->setType("MatrixadataArrive");
    //matrix->setX(20);
    //matrix->setY(2000);
    //matrix->setChunkLength(inet::B(2+sizeof(int)+sizeof(int)+1));
    // auto nouse = inet::makeShared<Matrix>();
    // nouse->setX(1);
    // nouse->setChunkLength(inet::B(1));

    inet::Packet* packet1 = new inet::Packet("SubMatrix");
    inet::Packet* packet2 = new inet::Packet("SubMatrix");
    inet::Packet* packet3 = new inet::Packet("SubMatrix");
    // packet1->insertAtBack(nouse);
    packet1->insertAtBack(submatrix1);
    // packet1->insertAtBack(nouse);
    // nouse->setX(2);
    // packet2->insertAtBack(nouse);
    packet2->insertAtBack(submatrix1);
    // nouse->setX(3);
    // packet3->insertAtBack(nouse);
    packet3->insertAtBack(submatrix2);
    hostSocket.sendTo(packet1, L3AddressResolver().resolve("192.168.4.2"),4002);
    hostSocket.sendTo(packet2, L3AddressResolver().resolve("192.168.5.2"),4002);
    hostSocket.sendTo(packet3, L3AddressResolver().resolve("192.168.6.2"),4002);

    EV << "UEWarningAlertApp::sendSubMatrix() - sent submatrix to the worker MEC app" << endl;
}

bool MECWarningAlertApp::collect(omnetpp::cMessage *msg){
    //TODO if the data is enough,send Result;
    //sendResultToUe();
    numOfSubResult++;
    auto pk = check_and_cast<Packet *>(msg);
    // auto sequence = dynamicPtrCast<const Result>(pk->peekAtFront<Result>());
    auto subResult = dynamicPtrCast<const BytesChunk>(pk->peekAtFront<BytesChunk>());
    // if(sequence->getRes() == 1 || sequence->getRes() == 2){
    //     if(numOfSubResult != 1){result.insert(result.end(),subResult->getBytes().begin(),subResult->getBytes().end());}//change 20000 element at front
    //     else
    //     result.insert(result.end(),subResult->getBytes().begin(),subResult->getBytes().end());
    // }
    // else{
    //     if(numOfSubResult == 1) result.insert(result.end(),subResult->getBytes().begin(),subResult->getBytes().end());
    //     else result.insert(result.end(),subResult->getBytes().begin(),subResult->getBytes().end());//change subResult then insert
    // }
    
    if (numOfSubResult == 2) return true;
    else return false;
}

void MECWarningAlertApp::sendResultToUe(){
    EV << "MECWarningAlertApp::sendResultToUe - send total result" << endl;

    auto info = inet::makeShared<BytesChunk>();
    result = std::vector<uint8_t>(40000,200);
    info->setBytes(result);
    info->addTagIfAbsent<inet::CreationTimeTag>()->setCreationTime(simTime());

    inet::Packet* packet = new inet::Packet("Matrix Result");
    packet->insertAtBack(info);
    ueSocket.sendTo(packet, ueAppAddress, ueAppPort);

}

void MECWarningAlertApp::handleMasterMessage(omnetpp::cMessage *msg){
    auto pk = check_and_cast<Packet *>(msg);
    ueAppAddress = pk->getTag<L3AddressInd>()->getSrcAddress();
    ueAppPort = pk->getTag<L4PortInd>()->getSrcPort();

    EV << "MECWarningAlertApp::handleMasterMessage - sub matrix data arrived" << endl;

    // auto seq = dynamicPtrCast<const Matrix>(pk->peekAtFront<Matrix>());
    auto submatrixPk = dynamicPtrCast<const BytesChunk>(pk->peekAtFront<BytesChunk>());
    if(submatrixPk == nullptr)
        throw cRuntimeError("MECWarningAlertApp::handleUeMessage - matrix data is null");

    // int ans = 0;
    // for(int i = 0; i<submatrixPk->getByteArraySize(); i++){
    //     ans += 2 * submatrixPk->getByte(i);
    // }

    auto info = inet::makeShared<BytesChunk>();
    // if(seq->getX() == 1 || seq->getX() == 2) 
    info->setBytes(std::vector<uint8_t>(1200,120));
    // else info->setBytes(std::vector<uint8_t>(20000,200));
    // auto nouse = inet::makeShared<Result>();
    // nouse->setRes(seq->getX());
    // nouse->setChunkLength(B(1));
    // info->setType("submatrix result");
    // info->setChunkLength(inet::B(sqrt(1024)));
    // info->addTagIfAbsent<inet::CreationTimeTag>()->setCreationTime(simTime());
    // info->setRes(ans);

    inet::Packet* packet = new inet::Packet("SubResult");
    // packet->insertAtBack(nouse);
    packet->insertAtBack(info);
    ueSocket.sendTo(packet, ueAppAddress, ueAppPort);
    
}

void MECWarningAlertApp::handleUeMessage(omnetpp::cMessage *msg)
{
    // determine its source address/port
    //EV<<"just test"<<endl;
    auto pk = check_and_cast<Packet *>(msg);
    ueAppAddress = pk->getTag<L3AddressInd>()->getSrcAddress();
    ueAppPort = pk->getTag<L4PortInd>()->getSrcPort();
    //EV<< pk->peekAll() << "Packettest" <<endl; sequence chunk
    //EV<< pk->str()<< "Packettest"<< endl;
    //EV<< pk->getNumTags()<< "Packettest"<< endl;
    //for(int i = 0;i<pk->getNumTags();i++){
    //    EV<<pk->getTag(i)<<"---test"<<endl;
    //}

    //auto mecPk = pk->peekAtFront<WarningAppPacket>();
    //auto mecPk = pk->peekAtFront<BytesChunk>();


    //if(strcmp(mecPk->getType(), "MatrixadataArrive") == 0)
    //{
        EV << "MECWarningAlertApp::handleUeMessage - matrix data arrived" << endl;
        //auto matrixPk = dynamicPtrCast<const Matrix>(mecPk);
        auto matrixPk = dynamicPtrCast<const BytesChunk>(pk->peekAtFront<BytesChunk>());
        if(matrixPk == nullptr)
            throw cRuntimeError("MECWarningAlertApp::handleUeMessage - matrix data is null");
        //double time = vim->calculateProcessingTime(mecAppId, 150);
        //x = matrixPk->getX();
        //y = matrixPk->getY();
        int ans = 0;
        for(int i = 0; i<matrixPk->getByteArraySize(); i++){
            ans += 2 * matrixPk->getByte(i);
        }

        //int ans = x * y;

        auto info = inet::makeShared<Result>();
        info->setType("matrix result");
        info->setChunkLength(inet::B(sqrt(1024*5)));
        info->addTagIfAbsent<inet::CreationTimeTag>()->setCreationTime(simTime());
        info->setRes(ans);
        //EV<< info->str()<< endl;
        inet::Packet* packet = new inet::Packet("Matrix Result");
        packet->insertAtBack(info);
        ueSocket.sendTo(packet, ueAppAddress, ueAppPort);
        // ueSocket.sendTo(test,L3AddressResolver().resolve("192.168.4.2"),4001);

//        auto info = inet::makeShared<Result>();
//        info->addTagIfAbsent<inet::CreationTimeTag>()->setCreationTime(simTime());
//        info->setType("matrix result");
//        info->setRes(ans);
//        inet::Packet* packet = new inet::Packet("Result");
//        packet->insertAtBack(info);
//        EV << info;
//        EV << packet;
//        ueSocket.sendTo(packet, ueAppAddress, ueAppPort);
    //}

    /*if(strcmp(mecPk->getType(), START_WARNING) == 0)
    {
        /*
         * Read center and radius from message

        EV << "MECWarningAlertApp::handleUeMessage - WarningStartPacket arrived" << endl;
        auto warnPk = dynamicPtrCast<const WarningStartPacket>(mecPk);
        if(warnPk == nullptr)
            throw cRuntimeError("MECWarningAlertApp::handleUeMessage - WarningStartPacket is null");

        centerPositionX = warnPk->getCenterPositionX();
        centerPositionY = warnPk->getCenterPositionY();
        radius = warnPk->getRadius();

        if(par("logger").boolValue())
        {
            ofstream myfile;
            myfile.open ("example.txt", ios::app);
            if(myfile.is_open())
            {
                myfile <<"["<< NOW << "] MEWarningAlertApp - Received message from UE, connecting to the Location Service\n";
                myfile.close();
            }
        }

        cMessage *m = new cMessage("connectService");
        scheduleAt(simTime()+0.005, m);
    }
    else if (strcmp(mecPk->getType(), STOP_WARNING) == 0)
    {
        sendDeleteSubscription();
    }

    else
    {
        throw cRuntimeError("MECWarningAlertApp::handleUeMessage - packet not recognized");
    }
    */
}

/*void MECWarningAlertApp::modifySubscription()
{
    std::string body = "{  \"circleNotificationSubscription\": {"
                       "\"callbackReference\" : {"
                        "\"callbackData\":\"1234\","
                        "\"notifyURL\":\"example.com/notification/1234\"},"
                       "\"checkImmediate\": \"false\","
                        "\"address\": \"" + ueAppAddress.str()+ "\","
                        "\"clientCorrelator\": \"null\","
                        "\"enteringLeavingCriteria\": \"Leaving\","
                        "\"frequency\": 5,"
                        "\"radius\": " + std::to_string(radius) + ","
                        "\"trackingAccuracy\": 10,"
                        "\"latitude\": " + std::to_string(centerPositionX) + ","           // as x
                        "\"longitude\": " + std::to_string(centerPositionY) + ""        // as y
                        "}"
                        "}\r\n";
    std::string uri = "/example/location/v2/subscriptions/area/circle/" + subId;
    std::string host = serviceSocket_.getRemoteAddress().str()+":"+std::to_string(serviceSocket_.getRemotePort());
    Http::sendPutRequest(&serviceSocket_, body.c_str(), host.c_str(), uri.c_str());
}

void MECWarningAlertApp::sendSubscription()
{
    std::string body = "{  \"circleNotificationSubscription\": {"
                           "\"callbackReference\" : {"
                            "\"callbackData\":\"1234\","
                            "\"notifyURL\":\"example.com/notification/1234\"},"
                           "\"checkImmediate\": \"false\","
                            "\"address\": \"" + ueAppAddress.str()+ "\","
                            "\"clientCorrelator\": \"null\","
                            "\"enteringLeavingCriteria\": \"Entering\","
                            "\"frequency\": 5,"
                            "\"radius\": " + std::to_string(radius) + ","
                            "\"trackingAccuracy\": 10,"
                            "\"latitude\": " + std::to_string(centerPositionX) + ","           // as x
                            "\"longitude\": " + std::to_string(centerPositionY) + ""        // as y
                            "}"
                            "}\r\n";
    std::string uri = "/example/location/v2/subscriptions/area/circle";
    std::string host = serviceSocket_.getRemoteAddress().str()+":"+std::to_string(serviceSocket_.getRemotePort());

    if(par("logger").boolValue())
    {
        ofstream myfile;
        myfile.open ("example.txt", ios::app);
        if(myfile.is_open())
        {
            myfile <<"["<< NOW << "] MEWarningAlertApp - Sent POST circleNotificationSubscription the Location Service \n";
            myfile.close();
        }
    }

    Http::sendPostRequest(&serviceSocket_, body.c_str(), host.c_str(), uri.c_str());
}

void MECWarningAlertApp::sendDeleteSubscription()
{
    std::string uri = "/example/location/v2/subscriptions/area/circle/" + subId;
    std::string host = serviceSocket_.getRemoteAddress().str()+":"+std::to_string(serviceSocket_.getRemotePort());
    Http::sendDeleteRequest(&serviceSocket_, host.c_str(), uri.c_str());
}
*/

void MECWarningAlertApp::established(int connId)
{
    if(connId == mp1Socket_.getSocketId())
    {
        EV << "MECWarningAlertApp::established - Mp1Socket"<< endl;
        // get endPoint of the required service
        const char *uri = "/example/mec_service_mgmt/v1/services?ser_name=LocationService";
        std::string host = mp1Socket_.getRemoteAddress().str()+":"+std::to_string(mp1Socket_.getRemotePort());

        Http::sendGetRequest(&mp1Socket_, host.c_str(), uri);
        return;
    }
    else if (connId == serviceSocket_.getSocketId())
    {
        EV << "MECWarningAlertApp::established - serviceSocket"<< endl;
        // the connectService message is scheduled after a start mec app from the UE app, so I can
        // response to her here, once the socket is established
        auto ack = inet::makeShared<WarningAppPacket>();
        ack->setType(START_ACK);
        ack->setChunkLength(inet::B(2));
        inet::Packet* packet = new inet::Packet("WarningAlertPacketInfo");
        packet->insertAtBack(ack);
        ueSocket.sendTo(packet, ueAppAddress, ueAppPort);
        //sendSubscription();
        return;
    }
    else
    {
        throw cRuntimeError("MecAppBase::socketEstablished - Socket %s not recognized", connId);
    }
}

void MECWarningAlertApp::handleMp1Message()
{
    EV << "MECWarningAlertApp::handleMp1Message - payload: " << mp1HttpMessage->getBody() << endl;

    try
    {
        nlohmann::json jsonBody = nlohmann::json::parse(mp1HttpMessage->getBody()); // get the JSON structure
        if(!jsonBody.empty())
        {
            jsonBody = jsonBody[0];
            std::string serName = jsonBody["serName"];
            if(serName.compare("LocationService") == 0)
            {
                if(jsonBody.contains("transportInfo"))
                {
                    nlohmann::json endPoint = jsonBody["transportInfo"]["endPoint"]["addresses"];
                    EV << "address: " << endPoint["host"] << " port: " <<  endPoint["port"] << endl;
                    std::string address = endPoint["host"];
                    serviceAddress = L3AddressResolver().resolve(address.c_str());;
                    servicePort = endPoint["port"];
                }
            }
            else
            {
                EV << "MECWarningAlertApp::handleMp1Message - LocationService not found"<< endl;
                serviceAddress = L3Address();
            }
        }

    }
    catch(nlohmann::detail::parse_error e)
    {
        EV <<  e.what() << std::endl;
        // body is not correctly formatted in JSON, manage it
        return;
    }

}

void MECWarningAlertApp::handleServiceMessage()
{
    if(serviceHttpMessage->getType() == REQUEST)
    {
        Http::send204Response(&serviceSocket_); // send back 204 no content
        nlohmann::json jsonBody;
        EV << "MEClusterizeService::handleTcpMsg - REQUEST " << serviceHttpMessage->getBody()<< endl;
        try
        {
           jsonBody = nlohmann::json::parse(serviceHttpMessage->getBody()); // get the JSON structure
        }
        catch(nlohmann::detail::parse_error e)
        {
           std::cout  <<  e.what() << std::endl;
           // body is not correctly formatted in JSON, manage it
           return;
        }

        if(jsonBody.contains("subscriptionNotification"))
        {
            if(jsonBody["subscriptionNotification"].contains("enteringLeavingCriteria"))
            {
                nlohmann::json criteria = jsonBody["subscriptionNotification"]["enteringLeavingCriteria"] ;
                auto alert = inet::makeShared<WarningAlertPacket>();
                alert->addTagIfAbsent<inet::CreationTimeTag>()->setCreationTime(simTime());
                alert->setType(WARNING_ALERT);

                if(criteria == "Entering")
                {
                    EV << "MEClusterizeService::handleTcpMsg - Ue is Entered in the danger zone "<< endl;
                    alert->setDanger(true);

                    if(par("logger").boolValue())
                    {
                        ofstream myfile;
                        myfile.open ("example.txt", ios::app);
                        if(myfile.is_open())
                        {
                            myfile <<"["<< NOW << "] MEWarningAlertApp - Received circleNotificationSubscription notification from Location Service. UE's entered the red zone! \n";
                            myfile.close();
                        }
                    }

                    // send subscription for leaving..
                    //modifySubscription();

                }
                else if (criteria == "Leaving")
                {
                    EV << "MEClusterizeService::handleTcpMsg - Ue left from the danger zone "<< endl;
                    alert->setDanger(false);
                    if(par("logger").boolValue())
                    {
                        ofstream myfile;
                        myfile.open ("example.txt", ios::app);
                        if(myfile.is_open())
                        {
                            myfile <<"["<< NOW << "] MEWarningAlertApp - Received circleNotificationSubscription notification from Location Service. UE's exited the red zone! \n";
                            myfile.close();
                        }
                    }
                    //sendDeleteSubscription();
                }

                alert->setPositionX(jsonBody["subscriptionNotification"]["terminalLocationList"]["currentLocation"]["x"]);
                alert->setPositionY(jsonBody["subscriptionNotification"]["terminalLocationList"]["currentLocation"]["y"]);
                alert->setChunkLength(inet::B(20));

                inet::Packet* packet = new inet::Packet("WarningAlertPacketInfo");
                packet->insertAtBack(alert);
                ueSocket.sendTo(packet, ueAppAddress, ueAppPort);

            }
        }
    }
    else if(serviceHttpMessage->getType() == RESPONSE)
    {
        HttpResponseMessage *rspMsg = dynamic_cast<HttpResponseMessage*>(serviceHttpMessage);

        if(rspMsg->getCode() == 204) // in response to a DELETE
        {
            EV << "MEClusterizeService::handleTcpMsg - response 204, removing circle" << rspMsg->getBody()<< endl;
             serviceSocket_.close();
             getSimulation()->getSystemModule()->getCanvas()->removeFigure(circle);

        }
        else if(rspMsg->getCode() == 201) // in response to a POST
        {
            nlohmann::json jsonBody;
            EV << "MEClusterizeService::handleTcpMsg - response 201 " << rspMsg->getBody()<< endl;
            try
            {
               jsonBody = nlohmann::json::parse(rspMsg->getBody()); // get the JSON structure
            }
            catch(nlohmann::detail::parse_error e)
            {
               EV <<  e.what() << endl;
               // body is not correctly formatted in JSON, manage it
               return;
            }
            std::string resourceUri = jsonBody["circleNotificationSubscription"]["resourceURL"];
            std::size_t lastPart = resourceUri.find_last_of("/");
            if(lastPart == std::string::npos)
            {
                EV << "1" << endl;
                return;
            }
            // find_last_of does not take in to account if the uri has a last /
            // in this case subscriptionType would be empty and the baseUri == uri
            // by the way the next if statement solve this problem
            std::string baseUri = resourceUri.substr(0,lastPart);
            //save the id
            subId = resourceUri.substr(lastPart+1);
            EV << "subId: " << subId << endl;

            circle = new cOvalFigure("circle");
            circle->setBounds(cFigure::Rectangle(centerPositionX - radius, centerPositionY - radius,radius*2,radius*2));
            circle->setLineWidth(2);
            circle->setLineColor(cFigure::RED);
            getSimulation()->getSystemModule()->getCanvas()->addFigure(circle);

        }
    }

}

void MECWarningAlertApp::handleSelfMessage(cMessage *msg)
{
    if(strcmp(msg->getName(), "connectMp1") == 0)
    {
        EV << "MecAppBase::handleMessage- " << msg->getName() << endl;
        connect(&mp1Socket_, mp1Address, mp1Port);
    }

    else if(strcmp(msg->getName(), "connectService") == 0)
    {
        EV << "MecAppBase::handleMessage- " << msg->getName() << endl;
        if(!serviceAddress.isUnspecified() && serviceSocket_.getState() != inet::TcpSocket::CONNECTED)
        {
            connect(&serviceSocket_, serviceAddress, servicePort);
        }
        else
        {
            if(serviceAddress.isUnspecified())
                EV << "MECWarningAlertApp::handleSelfMessage - service IP address is  unspecified (maybe response from the service registry is arriving)" << endl;
            else if(serviceSocket_.getState() == inet::TcpSocket::CONNECTED)
                EV << "MECWarningAlertApp::handleSelfMessage - service socket is already connected" << endl;
            auto nack = inet::makeShared<WarningAppPacket>();
            // the connectService message is scheduled after a start mec app from the UE app, so I can
            // response to her here
            nack->setType(START_NACK);
            nack->setChunkLength(inet::B(2));
            inet::Packet* packet = new inet::Packet("WarningAlertPacketInfo");
            packet->insertAtBack(nack);
            ueSocket.sendTo(packet, ueAppAddress, ueAppPort);

//            throw cRuntimeError("service socket already connected, or service IP address is unspecified");
        }
    }

    delete msg;
}
