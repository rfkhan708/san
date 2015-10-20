#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include "Structure.h"
#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include "TokenWiseClass.h"
#include <boost/thread/thread.hpp>
#include "Socket.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/lexical_cast.hpp>


#include <sstream>

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

using namespace std;

using namespace boost::signals2;

using namespace boost;

vector<struct Contract_File> cimlist;
map<int , TokenWiseClass*> _map;
boost::signals2::signal<void()> SigA;
boost::thread_group producer_threads;
const int MAXRCVSTRING = 255;

long long concat(long long x, long long y)
{
    long long temp = y;
    while (y != 0) {
        x *= 10;
        y /= 10;
    }
    return x + temp;
}

 void Eventsubscriber (boost::property_tree::ptree pt)
 {

    long ClientIdAuto=0;
    int sock;
	const char* addr = "ipc:///tmp/eventpubsub.ipc";

	cout << "Eventsubscriber Start: "<<addr<<"  ClientIdAuto: "<<ClientIdAuto<<endl;

	int* msg1;

	sock = nn_socket(AF_SP, NN_SUB);

	//int setBufSize= 32 * 1024 * 1024 ;

	//nn_setsockopt(sock,NN_SOL_SOCKET,NN_RCVBUF,&setBufSize,sizeof(setBufSize));

        cout << "After socket connection" << endl;
        std::string mx = pt.get<std::string>("DATA.MAXCLIENT");

        short MAXCLIENT =boost::lexical_cast<short>(mx) ;

        cout << "MAXCLIENT " << MAXCLIENT << endl<< endl;
        long lng=0;
        for(short imax = 0 ; imax <= MAXCLIENT; imax++)
        {
        //boost::lexical_cast<short>(
       // cout << endl <<"before CID " << endl;
            ClientIdAuto =boost::lexical_cast<long>(pt.get<std::string>("DATA.CLIENT" +boost::lexical_cast<std::string>(imax)));
            cout << "ClientIdAuto " << ClientIdAuto << endl<< endl;
            lng =concat((short)(MessageType)eFOPAIR,ClientIdAuto);
            nn_setsockopt(sock, NN_SUB,NN_SUB_SUBSCRIBE,&lng , sizeof(lng));
            lng =concat((short)(MessageType)eIOCPAIR,ClientIdAuto);
            nn_setsockopt(sock, NN_SUB,NN_SUB_SUBSCRIBE,&lng , sizeof(lng));
        }










	nn_connect(sock, addr) ;

	char _buffer[1024];
	char buffer[1024];

	short TransCode;

	 while (true)
	 {


        	memset(&_buffer,0,1024);

	        int size = nn_recv(sock, _buffer,1024, 0);

	        if(size<1)
            {
				cout << "Some error occured in eventubsub"<<endl<<endl;
			        continue;
            }



	 	 long suId;

       		suId=*(long*)_buffer;

            short MsgType =suId/1000000000000000;


	 	 memset(buffer,0,1024);
	 	 memcpy(buffer,_buffer+8,size-8);

		switch ((MessageType)MsgType)//(MessageType)BitConverter.ToInt16(_IncomingData, 0))
		{

			case (MessageType)eFOPAIR:
            {
                strFOPAIR _FOpairObj;
                memcpy(&_FOpairObj,buffer,sizeof(_FOpairObj));

                if(_map.find(_FOpairObj.TokenFar)==_map.end())
                {
                    _map[_FOpairObj.TokenFar]= new TokenWiseClass(_FOpairObj.TokenFar);
                    SigA.connect(bind(&TokenWiseClass::ClearData,_map[_FOpairObj.TokenFar]));
                }
                 if(_map.find(_FOpairObj.TokenNear)==_map.end())
                {
                    _map[_FOpairObj.TokenNear]= new TokenWiseClass(_FOpairObj.TokenNear);
                    SigA.connect(bind(&TokenWiseClass::ClearData,_map[_FOpairObj.TokenNear]));
                }

                cout << "FO Subscription recieved  " << endl;
            }
            case (MessageType)eIOCPAIR:
            {
                FOPAIRLEG2 _FOpairObj;
                memcpy(&_FOpairObj,buffer,sizeof(_FOpairObj));

                if(_map.find(_FOpairObj.Token1)==_map.end())
                {
                    _map[_FOpairObj.Token1]= new TokenWiseClass(_FOpairObj.Token1);
                    SigA.connect(bind(&TokenWiseClass::ClearData,_map[_FOpairObj.Token1]));
                }
                 if(_map.find(_FOpairObj.Token2)==_map.end())
                {
                    _map[_FOpairObj.Token2]= new TokenWiseClass(_FOpairObj.Token2);
                    SigA.connect(bind(&TokenWiseClass::ClearData,_map[_FOpairObj.Token2]));
                }
                if(_FOpairObj.Token3 > 0)
                if(_map.find(_FOpairObj.Token3)==_map.end())
                {
                    _map[_FOpairObj.Token3]= new TokenWiseClass(_FOpairObj.Token3);
                    SigA.connect(bind(&TokenWiseClass::ClearData,_map[_FOpairObj.Token3]));
                }
                if(_FOpairObj.Token4 > 0)
                if(_map.find(_FOpairObj.Token4)==_map.end())
                {
                    _map[_FOpairObj.Token4]= new TokenWiseClass(_FOpairObj.Token4);
                    SigA.connect(bind(&TokenWiseClass::ClearData,_map[_FOpairObj.Token4]));
                }
                cout << "IOC Subscription recieved  " << endl;

            }

				break;
			default:
				break;
		}


}
}


void BindStream(unsigned short StreamID, string multicastAddress,string LanIp,unsigned short port)
{

cout << " multicastAddress " << multicastAddress << " LanIp " << LanIp << " port " << port << " StreamID " << StreamID <<endl;
 //string multicastAddress ="239.70.70.21";//st_obj.multicastAddress-1; //"233.1.4.3";       // First arg:  multicast address
//  string LanIp="192.168.100.105";//st_obj.LanIp;//"192.168.168.36";
//  unsigned short port =4444;//atoi(st_obj.port);     // Second arg:  port

  try {
    UDPSocket sock(port);

    sock.joinGroup(multicastAddress,LanIp);

    char recvString[MAXRCVSTRING + 1];// Buffer for echo string + \0
    string sourceAddress;             // Address of datagram source
    unsigned short sourcePort;        // Port of datagram source

    //short msglen=0 ;
   // int seq=0;
    int LastSeq=0;
		STREAM_HEADER* _stmHd = (STREAM_HEADER*)calloc(8, sizeof(STREAM_HEADER));
		Order_Message* _OrdMsg = (Order_Message*)calloc(38, sizeof(Order_Message));
		Trade_Message* _TrdMsg = (Trade_Message*)calloc(45, sizeof(Trade_Message));

		for (;;)
		{
			int bytesRcvd = sock.recvFrom(recvString, MAXRCVSTRING, sourceAddress,
				sourcePort);
			recvString[bytesRcvd] = '\0';     // Terminate string

			memset(_stmHd, 0, sizeof(STREAM_HEADER));
			memcpy(_stmHd, recvString, sizeof(STREAM_HEADER));

			if(_stmHd->msg_len <=13)
			{
                cout << "HeartBeat recieved" << endl;
                /*FinalPrice _FPHB;
                memset (&_FPHB ,0, sizeof(FinalPrice));
                _FPHB.Token=111;
                _FPHB.sub_token=111;
                _FPHB.MAXBID=-1;
                _FPHB.MINASK=-1;
                _FPHB.LTP=-1;
                 nanoObj.SendData(_FPHB);*/
                continue;
            }

           // cout << "LastSeq " << LastSeq<< endl;
			if (LastSeq + 1 == _stmHd->seq_no)
			{
				LastSeq++;
				if (_stmHd->msg_len == 38)//NMX
				{
					memset(_OrdMsg, 0, sizeof(Order_Message));
					memcpy(_OrdMsg, recvString, sizeof(Order_Message));

					map<int, TokenWiseClass*>::iterator _val = _map.find(_OrdMsg->Token);

					if (_val != _map.end())
					{
					//cout << "NMX ORD NO " << (long long)_OrdMsg->Order_Id << " MsgType " << _OrdMsg->Message_Type << endl;
                   // cout<<"_val :"<<_val->second->Token<<endl;
						_val->second->BidAsk(_OrdMsg);


					}
					//delete _OrdMsg;
				}
				else if (_stmHd->msg_len == 45)//T
				{
					memset(_TrdMsg, 0, sizeof(Trade_Message));
					memcpy(_TrdMsg, recvString, sizeof(Trade_Message));

					map<int, TokenWiseClass*>::iterator _val = _map.find(_TrdMsg->Token);

					if (_val != _map.end())
					//if(_TrdMsg->Token==65937)
					{
					//	cout << "Trd ORD NO " << (long long)_TrdMsg->Buyorder_Id << " MsgType " << _TrdMsg->Message_Type << endl;
						_val->second->TrdMsg(_TrdMsg);

						//_map[_TrdMsg->Token]->TrdMsg(_TrdMsg);
					//	cout<<"event calling here..."<<endl;

					}
					//cout << "Trd ORD NO " << (long long)_TrdMsg->Buyorder_Id << " MsgType " << _TrdMsg->Message_Type << endl;
					//delete _TrdMsg;
				}
			}//Seq No Check
			else
			{
			 // SigA();
              LastSeq = _stmHd->seq_no;
         // cout << "Recovery occurred . Raising event to all classes to clear Holder" << endl;
        //Raise event from here to clear all the Data from Holder.
			}
			//cout << "Structure Data msg_len "<< _stmHd->msg_len << " SeqNo " << _stmHd->seq_no << " StreamID " << _stmHd->stream_id<<endl;
			//delete _stmHd;
			// cout << "Received " << recvString << " from " << sourceAddress << ": "
			// << sourcePort << endl;
		}
  }
   catch (SocketException &e)
  {
    cerr << e.what() << endl;
    //exit(1);
  }
}

string SECTION;
string CONTRACTFILEPATH;
string LanIP;
string MCASTIP;
string MPORT;
string counter;
string DATAIP;
short MCASTPORT;
boost::property_tree::ptree pt;

int main()
{
 boost::property_tree::ini_parser::read_ini("settings.ini", pt);
 producer_threads.add_thread(new boost::thread(Eventsubscriber,pt));

 sleep(2);




        SECTION = pt.get<std::string>("SECTION.ID");
      //CONTRACTFILEPATH =pt.get<std::string>(SECTION +".CONTRACTFILEPATH");
        DATAIP = pt.get<std::string>("DATA.BCASTADDR");
        LanIP = pt.get<std::string>("DATA.LANIP");
        //cout << "Generating Token List" << endl;
       // GenerateTokenList(CONTRACTFILEPATH);
        cout << "Binding class " << endl;
        //BindClass();
        nanoObj.BindNanoObj(DATAIP);
        cout <<"How many streams do you want to read ?"<< endl;

        int StreamCounter=0;

         cin >> StreamCounter;


        for(int icount = 0 ; icount < StreamCounter ; icount++)
        {
          // counter = patch::to_string(icount);
           counter = boost::lexical_cast<std::string>(icount);



            MCASTIP =  pt.get<std::string>("DATA.MCASTIP" + counter);



            MPORT=  pt.get<std::string>("DATA.MCASTPORT" + counter);



            MCASTPORT =boost::lexical_cast<short>(MPORT);


            producer_threads.add_thread(new boost::thread(BindStream,icount, MCASTIP ,LanIP,MCASTPORT));
            cout << " Thread " << icount << " started for port " << MPORT << endl;
            sleep(1);

        }

        //producer_threads.join_all();
//boost::thread_group producer_threads[4];

    cout << "joining all stream threads" << endl;
   producer_threads.join_all();




    return 0;
}