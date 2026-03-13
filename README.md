# IGraphics-TGame

IGraphics-TGame은 자체 3D 게임 엔진 및 에디터 개발 프로젝트입니다.   
현재 D3D11 기반 렌더러를 개발하고 있습니다.

### 🛠 사전 요구 사항
- C++ 컴파일러: C++17 이상 지원 (Visual Studio 2022 권장)
- CMake: 버전 3.16 이상
- OpenSSL v3.x: 시스템에 설치되어 있어야 합니다. (Win64 OpenSSL v3.x Full 버전 권장)
  
---

### 📦 프로젝트 구조
``` bash
ROOT
├── src
    ├── IdealGraphicsD3D11      # D3D11 렌더러
    ├── IdealGraphicsD3D11Demo  # D3D11 렌더러 데모
    ├── ModelConverter          # FBX -> 자체 데이터 컨버터
    └── Shader                  # 셰이더 코드
├── Extern                      # 외부 라이브러리
└── Resource                    # 리소스 루트 폴더
```

---

### 🔨 빌드 및 설치 방법
#### 간단 빌드 및 설치
저장소 루트에 있는 `BuildAll.bat` 을 실행하시면, 간단하게 빌드 및 설치됩니다.   
`BuildAllExtern.bat` 을 실행하시면, 사용하는 외부 라이브러리만 설치됩니다.

#### 직접 빌드
``` bash
git clone https://github.com/hedwig3798/FileManager.git

cd "[외부 라이브러리 설치 디렉토리]/build"

cmake .. ^
    -DEXT_INSTALL_DIR="[외부 라이브러리 설치 디렉토리]" ^
    -DBIN_OUTPUT_DIR="[외부 dll 설치 디렉토리]" ^
    -DBUILD_D3D11_PROJECT=ON ^
    -G "Visual Studio 17 2022" ^
    -A x64

cmake --build .

cmake .. ^
    -DEXT_INSTALL_DIR="[외부 라이브러리 설치 디렉토리]" ^
    -DBIN_OUTPUT_DIR="[외부 dll 설치 디렉토리]" ^
    -DRESOURCE_ROOT_PATH="[리소스 폴더]" ^
    -DCMAKE_INSTALL_LIBDIR="lib/$<CONFIG>" ^
    -DCMAKE_INSTALL_BINDIR="bin/$<CONFIG>" ^
    -G "Visual Studio 17 2022" ^
    -A x64
```
