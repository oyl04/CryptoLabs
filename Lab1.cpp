#include <bits/stdc++.h>

using namespace std;

bool isComposite[101];

vector <int> prime_list;

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

long long jacobi_symbol(long long a, long long n) {
    if (n <= 0 || n % 2 == 0) return 0;
    long long t = 1;
    a %= n;
    while (a != 0) {
        while (a % 2 == 0) {
            a /= 2;
            int r = n % 8;
            if (r == 3 || r == 5) {
                t = -t;
            }
        }
        swap(a, n);
        if (a % 4 == 3 && n % 4 == 3) {
            t = -t;
        }
        a %= n;
    }
    if (n == 1) return t;
    else return 0;
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

long long find_u(long long i, long long p, long long q, long long n) {

    long long u_prev = 0;
    long long u_cur = 1;
    if (i == 0) return u_prev;
    if (i == 1) return u_cur;
    long long u_next;
    for (int j = 2; j <= i; j++) {
        u_next = (p * u_cur - q * u_prev) % n;
        u_prev = u_cur;
        u_cur = u_next;
    }
    return u_next;
}


long long find_v(long long i, long long p, long long q, long long n) {

    long long v_prev = 2;
    long long v_cur = p;
    if (i == 0) return v_prev;
    if (i == 1) return v_cur;
    long long v_next;
    for (int j = 2; j <= i; j++) {
        v_next = (p * v_cur - q * v_prev) % n;
        v_prev = v_cur;
        v_cur = v_next;
    }
    return v_next;
}


bool lucas_test(long long n, long long d, long long p, long long q) {
    long long m = n + 1;
    long long r = 0;

    while (m % 2 == 0 && m > 0) {
        m /= 2;
        r++;
    }
    long long s = m;
    for (int j = 0; j < r; j++) {
        if (find_v(s * (1 << j), p, q, n) % n == 0) return true;
    }
    return find_u(s, p, q, n) % n == 0;
}

void build_prime_list(){
    prime_list.clear();
    for (int i = 2; i <= 100; ++i){
        if (isComposite[i]) continue;
        else prime_list.push_back(i);
        for (int j = i * i; j <= 100; j += i){
            isComposite[j] = 1;
        }
    }
}

string baillie_psw_test(long long n){
    build_prime_list();
    for (long long prime: prime_list){
        if (n == prime) return " is prime";
        else if (n % prime == 0) return " is not prime";
    }
    if (!miller_test(n - 1, n, 0)) return " is not prime";
    long long d = 5;
    while (jacobi_symbol(d, n) != -1) {
        if (d > 0)
            d = -(d + 2);
        else
            d = -(d - 2);
    }
    long long p = 1;
    long long q = (1 - d) / 4;
    if (lucas_test(n, d, p, q)) return " is probably prime";
    else return " is not prime";
}

string to_binary(long long x){
    if (x == 0) return "0";
    bool fl = 0, val;
    string res = "";
    for (int i = 62; i >= 0; --i){
        if (x >= (1ll << i)){
            x -= (1ll << i);
            val = 1;
            fl |= val;
        }
        else val = 0;
        if (fl) res += ('0' + val);
    }
    return res;
}

string to_decimal (long long x){
    return to_string(x);
}

string to_base64(long long x){
    string base2 = to_binary(x);
    reverse(base2.begin(), base2.end());
    while (base2.size() % 6 != 0){
        base2 += '0';
    }
    string res;
    reverse(base2.begin(), base2.end());
    for (int i = 0; i < base2.size(); i += 6){
        long long sum = 0;
        for (int j = 0; j < 6; ++j){
            sum += (base2[i + j] - '0') * (1 << (5 - j));
        }
        if (sum < 26) res += (sum + 'A');
        else if (sum < 52) res += (sum + 'a' - 26);
        else if (sum < 62) res += (sum + '0' - 52);
        else if (sum == 62) res += '+';
        else if (sum == 63) res += '/';
    }
    return res;
}

string to_bytes(long long x){
    string base2 = to_binary(x);
    reverse(base2.begin(), base2.end());
    while (base2.size() % 8 != 0){
        base2 += '0';
    }
    string res;
    reverse(base2.begin(), base2.end());
    for (int i = 0; i < base2.size(); i += 8){
        if (i != 0) res += ", ";
        else res += "[";
        long long sum = 0;
        for (int j = 0; j < 8; ++j){
            sum += (base2[i + j] - '0') * (1 << (7 - j));
        }
        res += to_string(sum);
    }
    res += "]";
    return res;
}

string formatting(long long x){
    while (true){
        cout << "Choose the format for the output:\n";
        cout << "1. base10\n";
        cout << "2. base2\n";
        cout << "3. base64\n";
        cout << "4. byte[]\n";
        int option;
        cin >> option;
        if (option == 1) return to_decimal(x);
        else if (option == 2) return to_binary(x);
        else if (option == 3) return to_base64(x);
        else if (option == 4) return to_bytes(x);
        else {
            cout << "Wrong number for the format! Try again\n";
            continue;
        }
        break;
    }
}

int main(){
    while(true){
    cout << "1. Binary exponentiation in modular arithmetic\n";
    cout << "2. Generate of N-bit prime number\n";
    cout << "3. Miller-Rabin primality test\n";
    cout << "4. Baillie-PSW primality test\n";
    cout << "0. Exit\n";
    cout << "Enter the number of command:\n";
    int option;
    cin >> option;
    if (option == 0){
        break;
    }
    else if (option == 1){
        long long base, mod, power;
        cout << "Enter the base:\n";
        cin >> base;
        cout << "Enter the power:\n";
        cin >> power;
        cout << "Enter the mod:\n";
        cin >> mod;
        long long res = binpow_mod(base, power, mod);
        string s = formatting(res);
        cout << "Result is equal to " << s << '\n';
    }
    else if (option == 2){
        int bits = 0;
        cout << "Enter the number of bits for prime number (from 2 to 63)\n";
        cin >> bits;

        while (bits <= 1 || bits >= 64){
            cout << "Invalid number of bits. Try again\n";
            cin >> bits;
        }
        long long res = generate_prime(bits);
        string s = formatting(res);
        cout << "Prime number with " << bits << " bits: " << s << '\n';
    }
    else if (option == 3){
        long long num;
        int k;
        cout << "Enter the number to primality test:\n";
        cin >> num;
        cout << "Enter the number of test rounds:\n";
        cin >> k;
        if (is_prime_miller_rabin(num, k)) {
            cout << num << " is probably prime\n";
            cout << "Accuracy is equal or more than:\n";
            long double acc = 1;
            while(k--){
                acc /= 4;
            }
            acc = 1 - acc;
            cout << fixed << setprecision(10) << acc << '\n';
        }
        else {
            cout << num << " is not prime\n";
        }
    }
    else if (option == 4){
        long long num;
        cout << "Enter the number to primality test:\n";
        cin >> num;
        cout << num << baillie_psw_test(num) << "\n";
    }
    else {
        cout << "Wrong number. Try again\n";
    }
    }
    return 0;
}
