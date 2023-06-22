#pragma once

#include <xaudio2.h>
#include <wrl.h>
#include <fstream>

#pragma comment(lib,"xaudio2.lib")

class Audio {
	//省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://構造体
	//チャンクヘッダ
	struct ChunkHeader {
		char id_[4];//チャンクのID
		int32_t size_;//チャンクサイズ
	};

	//RIFFヘッダファイル
	struct RiffHeader {
		ChunkHeader chunk_;//"RIFF"
		char type_[4];//"WAVE"
	};

	//FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk_;//"fmt "
		WAVEFORMATEX fmt_;//波形フォーマット
	};

	//音声データ
	struct SoundData {
		//波形フォーマット
		WAVEFORMATEX wfex_;
		//バッファの先頭アドレス
		BYTE* pBuffer_;
		//バッファのサイズ
		unsigned int bufferSize_;
	};

public://メンバ関数
	void Initialize();
	void Finalize();

	//音声再生
	void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData);

	//音声データ読み込み
	SoundData SoundLoadWave(const char* filename);
	//音声データ解放
	void SoundUnload(SoundData* soundData);

	//XAudio2取得
	ComPtr<IXAudio2> GetXAudio2() { return xAudio2_; }

private:
	ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_{};

private:
	Audio() = default;
	~Audio() = default;
	Audio(const Audio&) = delete;
	const Audio& operator=(const Audio&) = delete;
public:
	static Audio* GetInstance();
};