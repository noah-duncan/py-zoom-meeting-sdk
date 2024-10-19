#include "ZoomSDKAudioRawDataDelegate.h"


ZoomSDKAudioRawDataDelegate::ZoomSDKAudioRawDataDelegate(bool useMixedAudio = true, bool transcribe = false) : m_useMixedAudio(useMixedAudio), m_transcribe(transcribe){
    server.start();
}

void ZoomSDKAudioRawDataDelegate::onMixedAudioRawDataReceived(AudioRawData *data) {
    if (!m_useMixedAudio) return;

    // write to socket
    if (m_transcribe) {
        server.writeBuf(data->GetBuffer(), data->GetBufferLen());
        return;
    }

    // or write to file
    if (m_dir.empty())
        return Log::error("Output Directory cannot be blank");


    if (m_filename.empty())
        m_filename = "test.pcm";


    stringstream path;
    path << m_dir << "/" << m_filename;

    writeToFile(path.str(), data);
}



void ZoomSDKAudioRawDataDelegate::onOneWayAudioRawDataReceived(AudioRawData* data, uint32_t node_id) {
    if (m_useMixedAudio) return;

    stringstream path;
    path << m_dir << "/node-" << node_id << ".pcm";
    writeToFile(path.str(), data);
}

void ZoomSDKAudioRawDataDelegate::onShareAudioRawDataReceived(AudioRawData* data) {
    stringstream ss;
    ss << "Shared Audio Raw data: " << data->GetBufferLen() / 10 << "k at " << data->GetSampleRate() << "Hz";
    Log::info(ss.str());
}


void ZoomSDKAudioRawDataDelegate::writeToFile(const string &path, AudioRawData *data)
{
    static std::ofstream file;
	file.open(path, std::ios::out | std::ios::binary | std::ios::app);

	if (!file.is_open())
        return Log::error("failed to open audio file path: " + path);
	
    file.write(data->GetBuffer(), data->GetBufferLen());

    file.close();
	file.flush();

    stringstream ss;
    ss << "Writing " << data->GetBufferLen() << "b to " << path << " at " << data->GetSampleRate() << "Hz";

    Log::info(ss.str());
}

void ZoomSDKAudioRawDataDelegate::setDir(const string &dir)
{
    m_dir = dir;
}

void ZoomSDKAudioRawDataDelegate::setFilename(const string &filename)
{
    m_filename = filename;
}
