#include "Audio.h"
#include <cassert>

void Audio::Initialize() {
	HRESULT result;
	//XAudioエンジンのインスタンス生成
	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	SUCCEEDED(result);
	
	//マスタリングボイス生成
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
	SUCCEEDED(result);
}

void Audio::Finalize() {
	//解放
	xAudio2_.Reset();
}

void Audio::SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData) {
	HRESULT result;

	//波形フォーマットを元にソースボイスを生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex_);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer_;
	buf.AudioBytes = soundData.bufferSize_;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}

Audio::SoundData Audio::SoundLoadWave(const char* filename) {
#pragma region OpenFile
	std::ifstream file;
	//wavをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	//ファイルオープン失敗を検出
	assert(file.is_open());
#pragma endregion

#pragma region LoadWave
	//RIFFヘッダー読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFがチェック
	if (strncmp(riff.chunk_.id_, "RIFF", 4) != 0) {
		assert(0);
	}
	//ファイルがWAVEがチェック
	if (strncmp(riff.type_, "WAVE", 4) != 0) {
		assert(0);
	}

	//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダー確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if(strncmp(format.chunk_.id_,"fmt ",4) != 0) {
		assert(0);
	}
	assert(format.chunk_.size_ <= sizeof(format.fmt_));
	file.read((char*)&format.fmt_, format.chunk_.size_);

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出したら
	if (strncmp(data.id_, "JUNK", 4) == 0) {
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size_,std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id_, "data", 4) != 0) {
		assert(0);
	}

	//Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[static_cast<size_t>(data.size_)];
	file.read(pBuffer, data.size_);

	//Waveファイルを閉じる
	file.close();

#pragma endregion

	//返す音声データ
	SoundData soundData = {};

	soundData.wfex_ = format.fmt_;
	soundData.pBuffer_ = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize_ = static_cast<unsigned int>(data.size_);

	return soundData;
}

void Audio::SoundUnload(SoundData* soundData) {
	//バッファのメモリを解放
	delete[] soundData->pBuffer_;

	soundData->pBuffer_ = 0;
	soundData->bufferSize_ = 0;
	soundData->wfex_ = {};
}

Audio* Audio::GetInstance() {
	static Audio instance;
	return &instance;
}
