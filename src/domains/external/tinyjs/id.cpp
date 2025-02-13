#include "id.hpp"
#include <chrono>

using namespace std;
int objectId2 = 0;
string createUniqueId() {
  return to_string(
		chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count()
	)+"."+std::to_string(objectId2++);
}
