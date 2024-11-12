서버 프로젝트 설정

OpenSSL/BOOST를 위해서는 vcpkg를 설치하고 라이브러리를 받아야 한다.
스텝은 다음과 같으며 콘솔창은 관리자권한이 있는 상태에서 작업해야 한다.

1. vcpkg install boost:x64-windows 설치
   A. 프로젝트 내의 폴더가 아닌 특정 폴더에 VCPKG를 Git으로 다운로드 받는다.
      *VCPKG는 이후 다른 프로젝트에서 사용가능하다.
      -------------------------------------------------------------------------
      git clone https://github.com/microsoft/vcpkg.git
      -------------------------------------------------------------------------

   B. 다운로드가 완료되면  vcpkg 폴더로 이동하여 배치파일을 실행한다.
      -------------------------------------------------------------------------
      .\bootstrap-vcpkg.bat
      -------------------------------------------------------------------------


2. OpenSSL,BOOST,WebSocketpp 설치
      -------------------------------------------------------------------------
      vcpkg install openssl:x64-windows
      vcpkg install websocketpp
      vcpkg install flatbuffers
      vcpkg install boost:x64-windows
      vcpkg install boost:x64-windows-static
      -------------------------------------------------------------------------


3. vcpkg를 경로에 통합
      -------------------------------------------------------------------------
      .\vcpkg integrate install
      -------------------------------------------------------------------------


4. 프로젝트에서 OpenSSL 라이브러리 링크 확인:
    Visual Studio에서 프로젝트를 열고, Project -> Properties로 이동합니다.
    C/C++ -> General -> Additional Include Directories에서 vcpkg의 include 경로가 자동으로 추가되어 있는지 확인합니다.
    Linker -> General -> Additional Library Directories에서 OpenSSL 라이브러리가 있는 vcpkg의 lib 경로가 추가되어 있는지 확인합니다.


## .\vcpkg list 명령을 써서 설치된 리스트를 볼 수 있다.