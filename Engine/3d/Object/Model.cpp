#include "Model.h"
#include <cassert>
#include <fstream>
#include <sstream>
using namespace std;

template <class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

//デバイス
ComPtr<ID3D12Device> Model::device_ = nullptr;
// デスクリプタサイズ
UINT Model::descriptorIncrementSize_;

const std::string Model::Directory_ = "Resource/";

void Model::StaticInitialize(ID3D12Device* device) {
	Model::device_ = device;

	// メッシュの静的初期化
	Mesh::StaticInitialize(device);
}

Model* Model::LoadFromOBJ(const std::string& modelname, bool smoothing) {
	//インスタンス
	Model* model = new Model();
	//読み込み
	model->LoadFromOBJInternal(modelname, smoothing);

	//デスクリプタヒープの生成
	model->InitializeDescriptorHeap();

	model->LoadTextures();

	return model;
}

Model::~Model() {
	for (auto m : meshes_) {
		delete m;
	}
	meshes_.clear();

	for (auto m : materials_) {
		delete m.second;
	}
	materials_.clear();
}

void Model::LoadFromOBJInternal(const std::string& modelname, bool smoothing) {
	//ファイルストリーム
	std::ifstream file;
	//モデル名
	const string modelFilename = modelname + ".obj"; // "modelname.obj"
	const string directoryPath = Directory_ + modelname + "/"; // "Resources/modelname/"

															   //objファイルを開く
	file.open(directoryPath + modelFilename);

	//ファイルオープンの失敗を確認
	assert(!file.fail());

	name_ = modelname;

	// メッシュ生成
	Mesh* mesh = new Mesh;
	int indexCountTex = 0;
	//int indexCountNoTex = 0;

	vector<Vector3>positions;//頂点座標
	vector<Vector3>normals;//法線ベクトル
	vector<Vector2>texcoords;//テクスチャUV
							 //1行ずつ読み込む
	string line;
	while (getline(file, line)) {
		//1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		//半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		//マテリアル
		if (key == "mtllib") {
			// マテリアルのファイル名読み込み
			string materialFilename;
			line_stream >> materialFilename;
			// マテリアル読み込み
			LoadMaterial(directoryPath, materialFilename);
		}
		// 先頭文字列がgならグループの開始
		if (key == "g") {
			// グループ名読み込み
			string groupName;
			line_stream >> groupName;

			// メッシュに名前をセット
			mesh->SetName(groupName);
		}
		//先頭文字列がvなら頂点座標
		if (key == "v") {
			//X,Y,Z座標読み込み
			Vector3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			//座標データに追加
			positions.emplace_back(position);
		}
		//先頭文字列がvtならテクスチャ
		if (key == "vt") {
			//U,V成分読み込み
			Vector2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;
			//V方向反転
			texcoord.y = 1.0f - texcoord.y;
			//テクスチャ座標データに追加
			texcoords.emplace_back(texcoord);
		}
		//先頭文字列がvnなら法線ベクトル
		if (key == "vn") {
			//X,Y,Z座標読み込み
			Vector3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			//法線ベクトルデータに追加
			normals.emplace_back(normal);
		}
		// 先頭文字列がusemtlならマテリアルを割り当てる
		if (key == "usemtl") {
			if (mesh->GetMaterial() == nullptr) {
				// マテリアルの名読み込み
				string materialName;
				line_stream >> materialName;

				// マテリアル名で検索し、マテリアルを割り当てる
				auto itr = materials_.find(materialName);
				if (itr != materials_.end()) {
					mesh->SetMaterial(itr->second);
				}
			}
		}
		//先頭文字列がfならポリゴン(三角形)
		if (key == "f") {
			int faceIndexCount = 0;
			//半角スペース区切りで行の続きを読み込む
			string index_string;
			while (getline(line_stream, index_string, ' ')) {
				//頂点インデックス1個分の文字列をストリームに変換して解析しやすくする
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				//頂点番号
				index_stream >> indexPosition;

				Material* material = mesh->GetMaterial();
				index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす

													 // マテリアル、テクスチャがある場合
				if (material && material->filename_.size() > 0) {
					index_stream >> indexTexcoord;
					index_stream.seekg(1, ios_base::cur);//スラッシュを飛ばす
					index_stream >> indexNormal;
					//頂点データの追加
					Mesh::VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - static_cast<const unsigned _int64>(1)];
					vertex.normal = normals[indexNormal - static_cast<const unsigned _int64>(1)];
					vertex.uv = texcoords[indexTexcoord - static_cast<const unsigned _int64>(1)];
					mesh->AddVertex(vertex);
					//エッジ平滑化のデータを追加
					if(smoothing) {
						mesh->AddSmoothData(indexPosition, (unsigned short)(mesh->GetVertexCount() - 1));
					}
				}
				//頂点インデックスに追加
				if (faceIndexCount >= 3) {
					// 四角形ポリゴンの4点目なので、
					// 四角形の0,1,2,3の内 2,3,0で三角形を構築する
					mesh->AddIndex((unsigned short)(indexCountTex - 1));
					mesh->AddIndex((unsigned short)indexCountTex);
					mesh->AddIndex((unsigned short)(indexCountTex - 3));
				}
				else {
					mesh->AddIndex((unsigned short)indexCountTex);
				}
				indexCountTex++;
				faceIndexCount++;
			}
		}
	}
	//ファイルを閉じる
	file.close();

	//頂点法線の平均によるエッジ平滑化
	if (smoothing) {
		mesh->CalculateSmoothVertexNormals();
	}

	// コンテナに登録
	meshes_.emplace_back(mesh);

	// メッシュのマテリアルチェック
	for (auto& m : meshes_) {
		// マテリアルの割り当てがない
		if (m->GetMaterial() == nullptr) {
			if (defaultMaterial_ == nullptr) {
				// デフォルトマテリアルを生成
				defaultMaterial_ = Material::Create();
				defaultMaterial_->name_ = "no material";
				materials_.emplace(defaultMaterial_->name_, defaultMaterial_);
			}
			// デフォルトマテリアルをセット
			m->SetMaterial(defaultMaterial_);
		}
	}

	// メッシュのバッファ生成
	for (auto& m : meshes_) {
		m->CreateBuffers();
	}

	// マテリアルの数値を定数バッファに反映
	for (auto& m : materials_) {
		m.second->Update();
	}
}

void Model::LoadTextures() {
	int textureIndex = 0;
	string directoryPath = Directory_ + name_ + "/";

	for (auto& m : materials_) {
		Material* material = m.second;

		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV =
			descHeap_->GetCPUDescriptorHandleForHeapStart();
		cpuDescHandleSRV.ptr +=
			(descriptorIncrementSize_ * textureIndex);

		D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV =
			descHeap_->GetGPUDescriptorHandleForHeapStart();
		gpuDescHandleSRV.ptr +=
			(descriptorIncrementSize_ * textureIndex);

		// テクスチャなし
		if (material->filename_.size() <= 0) {
			directoryPath = Directory_;
		}

		// テクスチャ読み込み
		material->LoadTexture(directoryPath, cpuDescHandleSRV, gpuDescHandleSRV);

		textureIndex++;
	}
}

void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename) {
	//ファイルストリーム
	std::ifstream file;
	//マテリアルファイルを開く
	file.open(directoryPath + filename);
	//ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	Material* material = nullptr;

	//1行ずつ読み込む
	string line;
	while (getline(file, line)) {
		//1行分の文字列をストリームに変換
		std::istringstream line_stream(line);
		//半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		//先頭のタブ文字は無視する
		if (key[0] == '\t') {
			key.erase(key.begin()); //先頭の文字を削除
		}

		//先頭文字列がnewmtlならマテリアル名
		if (key == "newmtl") {
			//マテリアル生成済みなら
			if (material) {
				AddMaterial(material);
			}

			//新しく生成
			material = Material::Create();

			//マテリアル名読み込み
			line_stream >> material->name_;
		}

		//先頭文字列がKaならアンビエント色
		if (key == "Ka") {
			line_stream >> material->ambient_.x;
			line_stream >> material->ambient_.y;
			line_stream >> material->ambient_.z;
		}
		//先頭文字列がKdならディフューズ色
		if (key == "Kd") {
			line_stream >> material->diffuse_.x;
			line_stream >> material->diffuse_.y;
			line_stream >> material->diffuse_.z;
		}
		//先頭文字列がKsならスペキュラー色
		if (key == "Ks") {
			line_stream >> material->specular_.x;
			line_stream >> material->specular_.y;
			line_stream >> material->specular_.z;
		}

		//先頭文字列がmap_Kdならテクスチャファイル名
		if (key == "map_Kd") {
			//テクスチャのファイル名読み込み
			line_stream >> material->filename_;

			// フルパスからファイル名を取り出す
			size_t pos1;
			pos1 = material->filename_.rfind('\\');
			if (pos1 != string::npos) {
				material->filename_ = material->filename_.substr(
					pos1 + 1, material->filename_.size() - pos1 - 1);
			}

			pos1 = material->filename_.rfind('/');
			if (pos1 != string::npos) {
				material->filename_ = material->filename_.substr(
					pos1 + 1, material->filename_.size() - pos1 - 1);
			}
		}
	}
	//ファイルを閉じる
	file.close();

	if (material) {
		// マテリアルを登録
		AddMaterial(material);
	}
}

void Model::InitializeDescriptorHeap() {
	HRESULT result = S_FALSE;

	// マテリアルの数
	size_t count = materials_.size();

	// デスクリプタヒープを生成	
	if (count > 0) {
		D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
		descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダから見えるように
		descHeapDesc.NumDescriptors = (UINT)count; // シェーダーリソースビューの数
		result = device_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap_));//生成
		if (FAILED(result)) {
			assert(0);
		}
	}

	// デスクリプタサイズを取得
	descriptorIncrementSize_ =
		device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void Model::Draw(ID3D12GraphicsCommandList* cmdList) {

	//デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap_.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	for (auto mesh : meshes_) {
		// シェーダリソースビューをセット
		mesh->Draw(cmdList);
	}
}