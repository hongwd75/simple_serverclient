���� ������Ʈ ����

OpenSSL/BOOST�� ���ؼ��� vcpkg�� ��ġ�ϰ� ���̺귯���� �޾ƾ� �Ѵ�.
������ ������ ������ �ܼ�â�� �����ڱ����� �ִ� ���¿��� �۾��ؾ� �Ѵ�.

1. vcpkg install boost:x64-windows ��ġ
   A. ������Ʈ ���� ������ �ƴ� Ư�� ������ VCPKG�� Git���� �ٿ�ε� �޴´�.
      *VCPKG�� ���� �ٸ� ������Ʈ���� ��밡���ϴ�.
      -------------------------------------------------------------------------
      git clone https://github.com/microsoft/vcpkg.git
      -------------------------------------------------------------------------

   B. �ٿ�ε尡 �Ϸ�Ǹ�  vcpkg ������ �̵��Ͽ� ��ġ������ �����Ѵ�.
      -------------------------------------------------------------------------
      .\bootstrap-vcpkg.bat
      -------------------------------------------------------------------------


2. OpenSSL,BOOST,WebSocketpp ��ġ
      -------------------------------------------------------------------------
      vcpkg install openssl:x64-windows
      vcpkg install websocketpp
      vcpkg install flatbuffers
      vcpkg install boost:x64-windows
      vcpkg install boost:x64-windows-static
      -------------------------------------------------------------------------


3. vcpkg�� ��ο� ����
      -------------------------------------------------------------------------
      .\vcpkg integrate install
      -------------------------------------------------------------------------


4. ������Ʈ���� OpenSSL ���̺귯�� ��ũ Ȯ��:
    Visual Studio���� ������Ʈ�� ����, Project -> Properties�� �̵��մϴ�.
    C/C++ -> General -> Additional Include Directories���� vcpkg�� include ��ΰ� �ڵ����� �߰��Ǿ� �ִ��� Ȯ���մϴ�.
    Linker -> General -> Additional Library Directories���� OpenSSL ���̺귯���� �ִ� vcpkg�� lib ��ΰ� �߰��Ǿ� �ִ��� Ȯ���մϴ�.


## .\vcpkg list ����� �Ἥ ��ġ�� ����Ʈ�� �� �� �ִ�.