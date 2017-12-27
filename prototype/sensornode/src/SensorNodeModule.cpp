/*
 * Módulo do Nó Sensor
 * 
 * @author Ricardo Diniz Caldas
 * @version v1.0
 */

#include "SensorNodeModule.h"

SensorNodeModule::SensorNodeModule(const int32_t &argc, char **argv) :
    TimeTriggeredConferenceClientModule(argc, argv, "sensornode"),
    m_id(getIdentifier()),
    m_sensor_type(),
    m_battery(100.00),
    m_status("low"),
    m_collected("low"),
    m_data_queue(),
    m_status_log(),
    m_ref() {}


SensorNodeModule::~SensorNodeModule() {}

// CONFIGURAÇÃO
void SensorNodeModule::setUp() {
    // m_sensor_type = m_id+1; // configuração do tipo de sensor

    clock_gettime(CLOCK_REALTIME, &m_ref); // referência para medidas de tempo  

    string path = "output/";
    string filename = "sensornode" + to_string(m_id);
    m_status_log.open(path + filename + "_status_log.csv");
    m_status_log << "Tref:," << (m_ref.tv_sec) << "," << (m_ref.tv_nsec/1E9) << "," << "=B1+C1\n" << "\n";
    m_status_log << "ID, Battery Level, Actual Sensor Risk Status, Collected Sensor Status, Data Sent?, T sensornode.collected (s), Tinst sensornode.collected (s)\n";
}

// DESTRUIÇÃO
void SensorNodeModule::tearDown() {
}

bool SensorNodeModule::controllerFSM(int t){
    bool exe = false;
    
    if(m_status=="low"){
        exe = (t>=10)?true:false;
    } else if (m_status=="moderate") {
        exe = (t>=5)?true:false;
    } else if (m_status=="high") {
        exe = (t>=1)?true:false;
    } 

    return exe;
}

string SensorNodeModule::generateData(string actual_status){

    string category;
    int p = (rand() % 100) + 1;

    if(actual_status == "low"){
        if(1 <= p && p <= 33) {
            category = "high";
        } else if (33 < p && p <= 66) {
            category = "moderate";
        } else {
            category = actual_status;
        }
    } else if(actual_status == "moderate") {
        if(1 <= p && p <= 33) {
            category = "high";
        } else if (33 < p && p <= 66) {
            category = "low";
        } else {
            category = actual_status;
        }
    } else if(actual_status == "high"){
        if(1 <= p && p <= 33) {
            category = "low";
        } else if (33 < p && p <= 66) {
            category = "moderate";
        } else {
            category = actual_status;
        }
    } else {
        category = actual_status;
    }

    return category;
}

string SensorNodeModule::statusAnalysis(string actual_status) {

    string new_status = actual_status;
    int l=0, m=0, h=0;
    int threshold=3;

    for(uint32_t i = 0; i < m_data_queue.size(); i++) {
        if(m_data_queue[i]=="low"){
            l++;
        } else if(m_data_queue[i]=="moderate") {
            m++;
        } else if (m_data_queue[i]=="high"){
            h++;
        }
    } 

     
    // Analisar 3 de 5
    if(l>=threshold){
        new_status = "low";
    } else if (m>=threshold) {
        new_status = "moderate";
    } else if (h>=threshold) {
        new_status = "high";
    } else {
        new_status = actual_status;
    }
    
    /*
    // Analisar maioria entre sensores
    int max = h;

    if(l > max) {
        new_status = "low";
    } else if (m > max) {
        new_status = "moderate";
    } else {
        new_status = "high";
    }
    */

    return new_status;
}

void SensorNodeModule::sendSensorData(SensorData sensordata){
    Container container(sensordata);
    getConference().send(container);
    CLOG1 << sensordata.toString() << " sent at " << TimeStamp().getYYYYMMDD_HHMMSS() << endl;
}

void SensorNodeModule::consumeBattery(double val){
    m_battery -= val;
}

timespec SensorNodeModule::elapsedTime(timespec &now, timespec &ref) {

    timespec diff;

    if ((now.tv_nsec - ref.tv_nsec) < 0) {
        diff.tv_sec = now.tv_sec - ref.tv_sec - 1;
        diff.tv_nsec = now.tv_nsec - ref.tv_nsec + 1000000000L;
    } else {
        diff.tv_sec = now.tv_sec - ref.tv_sec;
        diff.tv_nsec = now.tv_nsec - ref.tv_nsec;
    }

    return diff;
}

// CORPO
odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode SensorNodeModule::body() {

    timespec ts;        // timestamp do processador

    srand(time(NULL));  // raíz da função de randomização
    bool exe;           // variável do atuador
    int cycles = 0;     // contador de ciclos desde a ultima execução

    m_sensor_type = (rand() % 3) + 1;
    timespec t_crab = m_ref;
    string new_status;

    int c=0;
    bool sent=false;
    int cont=3;

    while (getModuleStateAndWaitForRemainingTimeInTimeslice() == odcore::data::dmcp::ModuleStateMessage::RUNNING) {
        
        cycles++;
        exe = controllerFSM(cycles);  // para execução com controlador
        //exe = true;                     // para execuçao sem controlador
        consumeBattery(0.001);

        if(exe){
            /*GERAR DADOS*/
            
            bool flag = false;
            //for(int i = 0; i < 5; i++){
                
                string collected;
                timespec et = elapsedTime(ts, t_crab);

                //cout << "DEBUG ET: " << ((double)((et.tv_sec) + (et.tv_nsec/1E9))) <<endl;
                if(((double)((et.tv_sec) + (et.tv_nsec/1E9))) <= 10){
                    collected = m_status;
                } else {
                    collected = generateData(m_status);
                    flag = true;
                }
                consumeBattery(0.01);

                if(m_data_queue.size()>=5){
                    m_data_queue.pop_front();
                } 

                m_data_queue.push_back(collected);
            //}

            cout << "COLLECTED DATA: " << collected << endl;

            /*CAPTURAR INSTANTE DO PROCESSADOR*/
            clock_gettime(CLOCK_REALTIME, &ts);

            /*ANALISAR DADOS*/
            new_status = statusAnalysis(m_status);
            consumeBattery(0.01);

            if(new_status != m_status){
                m_status = new_status;
                /*ENVIAR ESTADO*/
                SensorNodeModule::sendSensorData(SensorData(m_id, m_sensor_type, m_status, ts));
                consumeBattery(0.01);

                sent = true;

                if(flag){
                    t_crab = ts;
                    flag=false;
                }
            }


            cout << ++c << "- Actual status: " << m_status  << " | Data sampled: " << new_status <<  " at " << TimeStamp().getYYYYMMDD_HHMMSSms() << endl;

            //ID
            m_status_log << m_id << ",";

            //Battery Level
            m_status_log << m_battery << ",";

            //Actual Sensor Risk Status
            m_status_log << m_status << ",";

            //Collected Sensor Risk Status
            m_status_log << collected << ","; 

            //Data Sent?
            string kk = (sent)?"true":"false";
            m_status_log << kk << ",";

            //T sensornode.collected
            timespec tts = elapsedTime(ts,m_ref);
            m_status_log << (double)((tts.tv_sec) + (tts.tv_nsec/1E9)) << ",";

            //T sensnronode.collcted inst
            ++cont;
            m_status_log << "=H" << to_string(cont) << "+ I" << to_string(cont) << "," << (ts.tv_sec) << "," << (ts.tv_nsec/1E9) << "\n";
            
            sent=false;
            cycles = 0;
        }
    }
    
    return odcore::data::dmcp::ModuleExitCodeMessage::OKAY;
}