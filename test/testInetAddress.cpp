#include "eirian/InetAddress.hpp"

#include <iostream>

int main() {
  eirian::InetAddress test_addr("127.0.0.1", 8888);
  std::cout << "ip: " << test_addr.getIp()
            << ", len: " << *test_addr.getAddrLen()
            << ", port: " << test_addr.getPort()
            << '\n';
}
