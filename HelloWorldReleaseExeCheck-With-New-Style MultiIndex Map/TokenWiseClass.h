#ifndef TOKENWISECLASS_H_INCLUDED
#define TOKENWISECLASS_H_INCLUDED
#include "NanoMQSender.h"
#include "Structure.h"
//#include <map>
#include<iostream>
#include<string.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/algorithm/minmax.hpp>
#include <boost/algorithm/minmax_element.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index/mem_fun.hpp>

using namespace std;
using boost::multi_index::multi_index_container;
using boost::multi_index::ordered_non_unique;
using boost::multi_index::ordered_unique;
using boost::multi_index::indexed_by;
using boost::multi_index::member;
using boost::multi_index::nth_index;
using boost::multi_index::get;
using boost::multi_index::hashed_unique;
using boost::multi_index::hashed_non_unique;
using boost::multi_index::indexed_by;
using boost::multi_index::tag;
using boost::multi_index::const_mem_fun;

struct Data_entry {
	Order_Message _OM;
	int _Price() const { return _OM.Price; }
	struct ByPrice {};
    double Order_ID_() const {return _OM.Order_Id; }
    struct ByOrderID {};
	struct RecordChange : public std::unary_function<Data_entry,void> {
		Order_Message p; RecordChange(const Order_Message &_p) : p(_p) {}
		void operator()(Data_entry & r) { r._OM = p; }
	};

};

typedef boost::multi_index_container
<Data_entry,
	indexed_by
	<
		hashed_unique<
                    tag<Data_entry::ByOrderID>, const_mem_fun<Data_entry,double,&Data_entry::Order_ID_>
                     >,
        ordered_non_unique<
                            tag<Data_entry::ByPrice>, const_mem_fun<Data_entry,int,&Data_entry::_Price>
                     >
    >
> Data_set;


class DataSorter
{
    Data_set _store;
    typedef Data_set::index<Data_entry::ByOrderID>::type OrderList;
    typedef Data_set::index<Data_entry::ByPrice>::type PriceList;

OrderList & _OrderList;
PriceList & _PriceList;

OrderList::const_iterator _OrderIterator;
PriceList::const_iterator _PriceIterator;
//FinalPrice _RetVal;
public :

    DataSorter():_OrderList(_store.get<Data_entry::ByOrderID>()),
    _PriceList(_store.get<Data_entry::ByPrice>())
    {
        //memset(&_RetVal,0,sizeof(_RetVal));

    }

    void InsertRecord(Order_Message Omsg)
    {
        Data_entry _DataRec = { Omsg};
        _store.insert(_DataRec);
       // _RetVal.Token = Omsg.Token;
       // _RetVal.sub_token = Omsg.Token;
       cout << " New Holder requested for Token " << Omsg.Token<<endl<<endl;
    }

    void UpdateRecord(Order_Message Omsg)
    {
        _OrderIterator = _OrderList.find(Omsg.Order_Id);
        if(_OrderIterator!= _OrderList.end())
        {
            _OrderList.modify(_OrderIterator,Data_entry::RecordChange(Omsg));
           // cout << " Request arrived to modify Data for token " << Omsg.Token<<endl<<endl;
        }
        else
        {
            InsertRecord(Omsg);
        }
    }

    void RemoveRecord(double Order_Id)
    {
        _OrderIterator = _OrderList.find(Order_Id);
        if(_OrderIterator!= _OrderList.end())
        {
            _OrderList.erase(_OrderIterator);
        }
        else
        {
            cout << " Request arrived to Remove Data for OrderID " << Order_Id<<" but not found"<<endl<<endl;
        }

    }

    int GetBidRecord()
    {
        int MAXBID=0;
        for(PriceList::reverse_iterator it = _PriceList.rbegin(), it_end(_PriceList.rend()); it != it_end; ++it)
        {
           // if(MAXBID==0)
                MAXBID = it->_OM.Price;
                break;
           // std::cout << "Bid " << (long long)it->Order_Id  <<" "
            //     << it->Price_rec_ << "  "
             //    << it->Qty_rec_ << std::endl;

        }
        return MAXBID;

    }

    int GetAskRecord()
    {
        int MINASK=0;
         for(PriceList::iterator it = _PriceList.begin(), it_end(_PriceList.end()); it != it_end; ++it)
        {
           // if(MINASK==0)
               MINASK = it->_OM.Price;

               break;
        }
        return MINASK;

    }


};

class TokenWiseClass
{


    FinalPrice _FPbackup;

    DataSorter _BuySorter;
    DataSorter _SellSorter;



        FinalPrice _FP;

        int Token;
       double BOID;
       double SOID;
        bool BIDASKerror;

public:
    TokenWiseClass(int Tkn)
    {
        memset(&_FP,0,sizeof(_FP));
        //cout<<" Class Initiated for "<< Tkn << " Earlier " << Token<< endl;

        Token= Tkn;
        _FP.Token = Tkn;
        _FP.sub_token=Tkn;

        BOID=0;
        SOID=0;

    }


    void BidAsk(Order_Message *Omsg)
    {

	switch (Omsg->Order_Type)
	{
	case 'B':

		if (Omsg->Message_Type == 'N' )
		{
            _BuySorter.InsertRecord(*Omsg);
		}
        else if ( Omsg->Message_Type == 'M')
        {
            _BuySorter.UpdateRecord(*Omsg);
        }
		else if (Omsg->Message_Type == 'X')
        {
            _BuySorter.RemoveRecord(Omsg->Order_Id);
        }


		break;

	case 'S':
        if (Omsg->Message_Type == 'N' )
		{
            _SellSorter.InsertRecord(*Omsg);
		}
        else if ( Omsg->Message_Type == 'M')
        {
            _SellSorter.UpdateRecord(*Omsg);
        }
		else if (Omsg->Message_Type == 'X')
        {
            _SellSorter.RemoveRecord(Omsg->Order_Id);
        }

		break;
	  }

        _FP.MAXBID = _BuySorter.GetBidRecord();
		_FP.MINASK = _SellSorter.GetAskRecord();

            if(((_FP.MAXBID != _FPbackup.MAXBID) || (_FPbackup.MINASK != _FP.MINASK) ) && _FP.MAXBID > 0 && _FP.MINASK > 0)
            {
                if(_FP.MAXBID > _FP.MINASK)
                {
                   // cout << "Seding cancel request for Token " << _FP.Token<< endl;
                    FinalPrice _FPCancel;
                    memset(&_FPCancel,0, sizeof(FinalPrice));
                    _FPCancel.Token=_FP.Token;
                    _FPCancel.sub_token=111;
                    nanoObj.SendData(_FPCancel);

                        if(BIDASKerror)
                        {
                            //ClearData();
                            cout << "BA BID > ASK BIDASKerror Token "<< BIDASKerror <<  " "<< _FP.Token<<endl;
                        }
                        else
                            BIDASKerror = true;

                }
                else
                {
                    nanoObj.SendData(_FP);
                    //cout << " Token " << _FP.Token << " MAXBID " << _FP.MAXBID << " MINASK " << _FP.MINASK << " LTP " << _FP.LTP << endl<<endl;

                }
                 _FPbackup= _FP;
            }


        //cout<< "BIDASK " << "Token " << _FP.Token << " BID " << _FP.MAXBID << " ASK " << _FP.MINASK << endl;

        //cout << " " << endl;


        }




 void TrdMsg(Trade_Message *Tmsg)
    {

        _FP.LTP = Tmsg->Trade_Price;


           _BuySorter.RemoveRecord(Tmsg->Buyorder_Id);
           _SellSorter.RemoveRecord(Tmsg->Selloder_Id);

        _FP.MAXBID = _BuySorter.GetBidRecord();

        _FP.MINASK = _SellSorter.GetAskRecord();

        //cout<< "\t LTP " << "Token " << _FP.Token << " BID " << _FP.MAXBID << " ASK " << _FP.MINASK << endl;
        //cout << " " << endl;

        nanoObj.SendData(_FP);

        if(_FP.MAXBID > _FP.MINASK && _FP.MAXBID > 0 && _FP.MINASK> 0)
            cout << "LTP BID > ASK"<< endl;
            else
            {
                BIDASKerror = false;
               // cout << "Reset from LTP"<< endl<< endl;
            }

    }






void ClearData()
{
   // _BuySorter.clear();
   // _SellSorter.clear();
//    _SellPriceRecord_D.clear();
  //  _BuyPriceRecord_D.clear();
    cout <<"Data Cleared for Token " << _FP.Token <<endl;

}
~TokenWiseClass()
{
    //dtor
}
};

#endif // TOKENWISECLASS_H_INCLUDED
