#include<iostream>
#include<string>
#include<vector>
#include<filesystem>
#include<memory>
#include<fstream>
#include<sstream>
#include<ctime>
#include<map>
#include<utility>
#include<ios>
#include"DailyStockData.h"
#include"EoDStockData.h"
#include"Globals.h"
void procFile(const std::string& filepath, std::shared_ptr<std::map<std::string, EodStockData>>_sptr);
DailyOHLCV NSE_EOD_Data_store(std::vector<std::string>& eodData);

int main(int argc, char** argv){
/*
main map holding data of all stocks in a given exchange. Stock symbol(string) is the key
object of type EoDStockData is the value
*/
    std::shared_ptr<std::map<std::string, EoDStockData>> _sptrEoDStockData = std::make_shared<std::map<std::string, EoDStockData>>();

    //read EoD data files
    std::string filePath("c:\\apps\\H\\...");//insert path to firectory having EoD files
    std::filesystem::path csvdir{filePath};
    for(const auto& file:std::filesystem::directory_iterator(csvdir)){
        if(file.path().extension()==".csv"){
            procFile(file.path().string(),_sptrEoDStockData);
        }
    }
    for(auto itr=(*_sptrEoDStockData).begin(); itr!=(*_sptrEoDStockData).end();itr++)
        ((*itr).second).createIndicatorAndWeeklyData();
    return 0;
}//int main(int argc, char** argv)


void procFile(const std::string& filePath, std::shared_ptr<std::map<std::string,EoDStockData>> _sptrEoDStockData){
    std::ifstream csvfile{filePath};
    if(!csvfile.is_open()){
        std::cout<<"EoD file not open.";
        return;
    }
    std::string line, word
    std::getline(csvfile,line);//header row to discard.
    
    while(std::getline(csvfile,line)){ //keep reading each line in the csv
        std::stringstream strstream{line};
        std::vector<std::string> eodData;

        //insert each comma separated value into a vector as a separate string
        //Optimize, check if the value at position 1 in the "line" string is not "EQ" then this loop is not required
        while(std::getline(strstream,word,',')){
            eodData.push_back(word);
        }
        //Read only lines for equity
        if(eodData[1]="EQ"){ //applies to NSE
            EoDStockData stckData(eodData[0],Exchange::NSE);
            DailyOHLCV ohlc=NSE_EOD_Data_store(eodData);
            auto retval=_sptrEoDStockData->emplace(eodData[0],stckData);
            auto iter=retval.first;
            ((*iter).second).insertPriceData(ohlc);
        }
    }//while(std::getline(csvfile,line))
}//void procFile(...........)

//Exchange specific processor
DailyOHLCV NSE_EOD_Data_store(std::vector<std::string>& eodData){
    //Creating date label
    std::stringstream dtstream{eodData[10]};
    std::string yr, mon, dy;
    std::getline(dtstream,dy,'-');
    std::getline(dtstream,mon,'-');
    std::getline(dtstream,yr,'-');
    std::string monthNum{"00"};
    if(mon=="JAN")
        monthNum="01";
    if(mon=="Feb")
        monthNum="02";
    if(mon=="Mar")
        monthNum="03";
    if(mon=="Apr")
        monthNum="04";
    if(mon=="MAY")
        monthNum="05";
    if(mon=="JUN")
        monthNum="06";
    if(mon=="JUL")
        monthNum="07";
    if(mon=="AUG")
        monthNum="08";
    if(mon=="SEP")
        monthNum="09";
    if(mon=="OCT")
        monthNum="10";
    if(mon=="NOV")
        monthNum="11";
    if(mon=="DEC")
        monthNum="12";
    
    DailyOHLCV ohlc;
    ohlc.strDate=yr+monthNum+dy;
    ohlc.Open=std::stof(eodData[2]);
    ohlc.High=std::stof(eodData[3]);
    ohlc.Low=std::stof(eodData[4]);
    ohlc.Close=std::stof(eodData[5]);
    ohlc.prev_Close=std::stof(eodData[7]);
    ohlc.Volume=std::stof(eodData[8]);
    std::tm tm{};
    tm.tm_year = std::stoi(yr)-1900;
    tm.tm_mday = std::stoi(dy);
    tm.tm_mon = std::stoi(monthNum)-1; .// subtract 1 because tm stores month from 0 to 11
    std::mktime(&tm);
    std::ostringstream yrweekstream;
    yrweekstream<<yr<<std::put_time(&tm,"%W");
    ohlc.strYearWeek = yrweekstream.str();
    ohlc.strYearMonth = yr+monthNum;
    /*
        if first few days of a year are in the middle of the week,
        make them part of the last week of prev year
        we can't adjust in the insert function because the _priceData is not yet sorted 
    */
   if(ohlc.strYearWeek.ends_with("00")){
    std::tm tm{};
    tm.tm_year=std::stoi(yr)-1900-1;
    tm.tm_mday = 31;
    tm.tm_mon=11; //December, tm stores months from 0 to 11
    std::mktime(&tm);
    std::ostringstream yrweekstream;
    yrweekstream<<(std::stoi(yr)-1)<<std::put_time(&tm,"%W");
    ohlc.strYearWeek=yrweekstream.str();
   }
}//DailyOHLCV NSE_EOD_Data_store(std::vector<std::string>& eodData){
