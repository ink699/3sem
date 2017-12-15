#include <fstream>
#include <string>

using namespace std;

int main()
{
  string s = "10";
  for(int i = 10; i <= 25; ++i)
  {
    s[0] = (i / 10) + '0';
    s[1] = (i % 10) + '0';
    ofstream out(s);
    for(int j = 0; j < 1 << i; ++j)
    {
      out << "1";
    }
    out << endl;
  }
  return 0;
}
