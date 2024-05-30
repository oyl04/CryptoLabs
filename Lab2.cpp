#include <bits/stdc++.h>


using namespace std;

long long binpow_mod (long long a, long long b, long long mod){
    __int128_t temp = 0;
    a %= mod;
    long long res = 1;
    while (b){
        temp = (__int128_t)res * a;
        if (b % 2) res = temp % mod;
        temp = (__int128_t)a * a;
        a = temp % mod;
        b /= 2;
    }
    return res;
}

bool miller_test(long long d, long long n, bool random_base = 1)
{
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 mt_generator(seed);
    uniform_int_distribution<long long> distribution(2, n - 2);
    long long a;
    if (random_base) a = distribution(mt_generator);
    else a = 2;
    __int128_t x = binpow_mod(a, d, n);
    if (x == 1 || x == n - 1)
       return true;
    if (x < 0){
        return false;
    }
    while (d <= n - 1) {
        x = (x * x) % n;
        d *= 2;
        if (x == 1)
            return false;
        if (x == n - 1)
            return true;
    }
    return false;
}

bool is_prime_miller_rabin(long long n, int k)
{
    int pow = 0;
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0) return false;
    long long d = n - 1;
    while (d % 2 == 0) d /= 2, ++pow;
    for (int i = 0; i < k; i++){
         if (!miller_test(d, n)) return false;
    }
    return true;
}

string int_to_hex(unsigned int x)
{
  stringstream stream;
  stream << setfill ('0') << setw(8) << hex << x;
  return stream.str();
}


long long generate_prime(long long bits){
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    mt19937 mt_generator(seed);
    uniform_int_distribution<long long> distribution(1ll << (bits - 1), (1ll << bits) - 1);
    long long generated_number = 0;
    do {
        generated_number = distribution(mt_generator);
    }
    while(!is_prime_miller_rabin(generated_number, 30));
    return generated_number;
}

long long gcd(long long a, long long b){
    if (a == 0) return b;
    return gcd(b % a, a);
}

long long lcm (long long a, long long b){
    long long res = b / gcd(a, b) * a;
    return res;
}

long long carmichael (long long p, long long q){
    return lcm(p - 1, q - 1);
}


pair <long long, long long> extended_euclid(long long a, long long b) {
    long long bs = b;
    long long x = 0, y = 1, lastx = 1, lasty = 0;
    while (b != 0) {
        long long temp, q;
        q = a / b;
        temp = a % b;
        a = b;
        b = temp;
        temp = x;
        x = lastx - q * x;
        lastx = temp;
        temp = y;
        y = lasty - q * y;
        lasty = temp;
    }
    if (lastx < 0) lastx += bs;
    return {lastx, lasty};
}

long long chinese_remainder_theorem(vector<long long> r, vector<long long> a) {
    long long prod = 1;
    for (int i = 0; i < a.size(); ++i) prod *= a[i];
    long long res = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        long long M = prod / a[i];
        long long N = extended_euclid(M, a[i]).first;
        res += M * N * r[i];
    }
    return res % prod;
}

class RSA{
    int bits;
    long long n, p, q, e, d, lambda;
    public:
      long long decrypt (long long message){
          long long exp1 = d % (p - 1);
          long long exp2 = d % (lambda / (p - 1));
          long long mp = binpow_mod(message, exp1, p);
          long long mq = binpow_mod(message, exp2, q);
          vector <long long> r = {mp, mq};
          vector <long long> a = {p, q};
          return chinese_remainder_theorem(r, a);
      }

      long long encrypt (long long message){
          return binpow_mod(message, e, n);
      }
      RSA(int bits){
          p = generate_prime(bits);
          while (p == q || q == 0 || p == 0) {
                q = generate_prime(bits);
          }
          n = p * q;
          lambda = carmichael(p, q);
          do {e = generate_prime(bits);}
          while (gcd(e, lambda) != 1);
          d = extended_euclid(e, lambda).first;
      }
      RSA(long long p, long long q, int bits):
          p(p), q(q), bits(bits){
            n = p * q;
            lambda = carmichael(p, q);
            do {e = generate_prime(bits);}
            while (gcd(e, lambda) != 1);
          }
      bool is_valid(long long message){
          if (message >= n) return false;
          else return true;
      }

    unsigned int left_shift(unsigned int w, int bits) {
        return (w << bits) | (w >> (32 - bits));
    }
    string SHA1(string message) {
        unsigned int h[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
        vector<unsigned char> bits(message.begin(), message.end());
        unsigned long long ml = bits.size() * 8;
        bits.push_back(0x80);
        while (bits.size() % 64 != 56) bits.push_back(0x00);
        for (int i = 7; i >= 0; --i) bits.push_back((ml >> (i * 8)) & ((1 << 8) - 1));
        for (int i = 0; i < bits.size(); i += 64) {
            uint32_t w[80];
            for (int j = 0; j < 16; ++j) {
                w[j] = 0;
                for (int k = 0; k < 4; ++k) w[j] |= (unsigned int)(bits[i + j * 4 + k] << (24 - k * 8));
            }
            for (int j = 16; j < 80; ++j) w[j] = left_shift((w[j - 3] ^ w[j - 8] ^ w[j - 14] ^ w[j - 16]), 1);
            unsigned int a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];
            unsigned int k[4] = {0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6};
            for (int j = 0; j < 80; ++j) {
                unsigned int f;
                if (j < 20) f = (b & c) | (~b & d);
                else if (j < 40) f = b ^ c ^ d;
                else if (j < 60) f = (b & c) | (b & d) | (c & d);
                else f = b ^ c ^ d;
                int temp = left_shift(a, 5) + f + e + k[j / 20] + w[j];
                e = d;
                d = c;
                c = left_shift(b, 30);
                b = a;
                a = temp;
        }
        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
        h[4] += e;
    }
    string result;
    for (auto i : h) {
        result += int_to_hex(i);
    }
    return result;
}
};

int main(){
    while(true){
        int option;
        cout << "1. RSA\n";
        cout << "2. SHA-1\n";
        cout << "0. Exit\n";
        cout << "Enter the number of command:\n";
        cin >> option;
        if (option == 1){
        long long message;
        cout << "Enter the message:\n";
        RSA R = RSA(15);
        while (cin >> message){
            if (!R.is_valid(message)){
                cout << "Too large message! Try again\n";
            }
            else break;
        }
        long long e = R.decrypt(message);
        cout << message << '\n';
        long long d = R.encrypt(e);
        cout << e << '\n';
        cout << d << '\n';
        }
        else if (option == 2){
            cout << "Enter the message:\n";
            RSA R = RSA(32);
            string message;
            cin >> message;
            cout << "SHA-1:\n" << R.SHA1(message) << '\n';
        }
    }
    return 0;
}
