/*
 * Módulo do Nó Sensor
 * 
 * @author Ricardo Diniz Caldas
 * @version v1.0
 */

#ifndef BODYHUB_MODULE_H_
#define BODYHUB_MODULE_H_

#include "opendavinci/odcore/base/FIFOQueue.h"
#include "opendavinci/odcore/base/module/TimeTriggeredConferenceClientModule.h"
#include "openbasn/data/SensorData.h"

#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <map>

using namespace std;
using namespace odcore::base;
using namespace odcore::base::module;
using namespace openbasn::data;

class BodyHubModule : public TimeTriggeredConferenceClientModule {

    // Boas práticas pelo manual do OpenDaVINCI.
    private:      
        BodyHubModule(const BodyHubModule &/*obj*/);
        BodyHubModule& operator=(const BodyHubModule &/*obj*/); 

    // Declaração dos métodos abstratos da classe pai.
    public:
    odcore::data::dmcp::ModuleExitCodeMessage::ModuleExitCode body();
    
    private:
        virtual void setUp();
        virtual void tearDown();
    
    // Construtor e Destrutor da classe.
    public:
        BodyHubModule(const int32_t &argc, char **argv);
        virtual ~BodyHubModule();
    

    private:
        /**
         * Cálculo da diferença de tempo entre dois timespecs
         * (agora - referencia)
         * 
         * @param agora, referencia 
         * @return resultado da diferença
         */
        timespec elapsedTime(timespec &/*now*/, timespec &/*ref*/);

        /**
         * Cálculo do estado do paciente
         * 
         * @param  
         * @return novo estado do paciente
         */ 
        string calculateHealthStatus();

        /**
         * Atribui estado do nó sensor recebido no registro e
         * Atualiza o estado do paciente
         * 
         * @param mensagem com os dados do sensor
         * @return 
         */
        void updateHealthStatus(SensorData /*sensordata*/);

        /**
         * Persiste um snapshot dos dados do registro e 
         * informações do dado recebido dado
         * 
         * @param identificador do nó sensor remetente, instante 
         * de envio 
         * @return 
         */
        void persistHealthStatus(uint32_t /*sensor_id*/, timespec /*sent_timespec*/, timespec /*received_timestamp*/);
        
        /**
         * Imprime na tela o estado atual do registro de sensores 
         * 
         * @param 
         * @return 
         */
        void printHealthStatus();
    
    // Atributos da classe
    private:
        uint32_t m_id; // identificador
        FIFOQueue m_buffer; // fila de caontainers
        string m_health_status; // estado do paciente
        map<uint32_t, string> m_sensor; // registro de sensores e estado atual
        ofstream m_status_log; // arquivo para log
        timespec m_ref; // referencia temporal
};

#endif