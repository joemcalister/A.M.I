
#ifndef CI_FFTOctaveAnalysis
#define CI_FFTOctaveAnalysis

#include "fft.h"
#include "fft/fftOctaveAnalyzer.h"

#include "cinder/audio/audio.h"
#include "cinder/Easing.h"


class FFTOctaveAnalysis{
public:
    
    FFTOctaveAnalysis(){
        
    }
    
    ~FFTOctaveAnalysis(){
        
    }
    
    static FFTOctaveAnalysis& get()
    {
        static FFTOctaveAnalysis instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    
    void init(const ci::audio::NodeRef& inputNode){
        

        auto audioContext = ci::audio::Context::master();
        auto monitorFormat = ci::audio::MonitorSpectralNode::Format().fftSize( 1024 ).windowSize( 512 );
        mMonitorSpectralNode = audioContext->makeNode( new ci::audio::MonitorSpectralNode( monitorFormat ) );
        
        inputNode >> mMonitorSpectralNode;
        
        audioContext->enable();
        
        //  SETUP AUDIO 2
        int numOctaves=2;
        nBands = 9 * numOctaves + 1;
        

        memset(magnitude, 0, BUFFER_SIZE_FFT);
        memset(phase, 0, BUFFER_SIZE_FFT);
        memset(power, 0, BUFFER_SIZE_FFT);
        memset(freq, 0, BUFFER_SIZE_FFT);
        
        
        ci::audio::Buffer audioBuffer = ci::audio::Buffer(BUFFER_SIZE_FFT);
        
        FFTanalyzer.setup(audioContext->getSampleRate(), BUFFER_SIZE_FFT/2, numOctaves);
        FFTanalyzer.peakHoldTime = 2;
        FFTanalyzer.peakDecayRate = .89;
        FFTanalyzer.linearEQIntercept = 0.8f; // reduced gain at lowest frequency
        FFTanalyzer.linearEQSlope = 0.01f; // increasing gain at higher frequencies
        
        
        picos = std::vector<float>(FFTanalyzer.nAverages);
        
        for (auto & v : picos) {
            v = 0;
        }
    }
    
    void update(){
        
        audioBuffer = mMonitorSpectralNode->getBuffer();
        
        myfft.powerSpectrum(0,(int)BUFFER_SIZE_FFT/2, audioBuffer.getChannel(1), BUFFER_SIZE_FFT, &magnitude[0], &phase[0], &power[0], &avg_power);
        
        
        FFTanalyzer.calculate(magnitude);
        
        
        
        std::vector<float> peaks(FFTanalyzer.nAverages);
        memcpy(&peaks[0], FFTanalyzer.peaks, sizeof(float) * FFTanalyzer.nAverages);
        
        
        
        for(int i = 0; i< FFTanalyzer.nAverages; i++){
            
//            float audioValue =
            float diff = peaks[i] * audioGain - picos[i];
            picos[i] += diff * audioEasing; // (audioValue - ) * audioEasing;

            picos[i] = cinder::math<float>::clamp( picos[i] + audioPiso, 0.0f, audioMax);
            
//            picos[i] = cinder::easeInOutAtan(picos[i] / audioMax) * audioMax * 2;
            
        }
    }

    
    float getAverage(){
        
        return FFTanalyzer.averageFrequencyIncrement;
    }
    
    std::vector<float> getPicos(){
        return picos;
    }
    
    
    
    
private:
    
    // CI Audio
    ci::audio::MonitorSpectralNodeRef mMonitorSpectralNode;

    const static int BUFFER_SIZE_FFT = 512;
    
    // AUDIO 2 VARS
    int nBands = 60;
    float avg_power = 0.0f;
    std::vector<float> picos;
    int 	bufferCounter;
    
    FFTOctaveAnalyzer FFTanalyzer;
    
    
    fft		myfft;
    float magnitude[BUFFER_SIZE_FFT];
    float phase[BUFFER_SIZE_FFT];
    float power[BUFFER_SIZE_FFT];
    float freq[BUFFER_SIZE_FFT/2];
    ci::audio::Buffer audioBuffer = ci::audio::Buffer(BUFFER_SIZE_FFT);
    
    
    float audioEasing = 0.85f;
    float audioGain = 1.0f;
    
    float audioPiso = 0.0f;
    float audioMax  = 100.0;
    
};


#endif