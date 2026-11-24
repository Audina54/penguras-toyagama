// E-Commerce Dynamic Filter Engine
// Kelompok Pengurus Toyagama
//
// Program membaca katalog produk Amazon dari amazon_clean.csv, lalu lewat menu
// bisa dipakai untuk mengurutkan katalog berdasarkan skor relevansi (Quick Sort
// buatan sendiri) atau mencari produk berdasarkan ID (Interpolation Search).
// Alurnya mengikuti flowchart: muat data, menu utama, lalu opsi 1, 2, atau 3 (keluar).
//
// Kolom CSV: title, final_price, reviews_count, rating.
// reviews_count dipakai sebagai jumlah "Terjual".

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

// Berapa produk teratas yang ditampilkan setelah pengurutan (Opsi 1).
// Kalau mau menampilkan semuanya, isi dengan angka yang sama atau lebih besar dari jumlah produk.
const int JUMLAH_TAMPIL = 10;

struct Produk {
    int id;
    string nama;
    double harga;
    double rating;
    int terjual;                   // diambil dari kolom reviews_count
    double Skor_Relevansi_Produk;  // diisi oleh Hitung_Skor_Relevansi
};

// Fungsi bantu

// Buang spasi di ujung teks, \r sisa file CSV dari Windows, dan tanda kutip pembungkus.
string Bersihkan_Teks(string teks) {
    while (!teks.empty() && (teks.back() == '\r' || teks.back() == '\n' || teks.back() == ' '))
        teks.pop_back();
    size_t awal = 0;
    while (awal < teks.size() && teks[awal] == ' ') awal++;
    teks = teks.substr(awal);
    if (teks.size() >= 2 && teks.front() == '"' && teks.back() == '"')
        teks = teks.substr(1, teks.size() - 2);
    return teks;
}

// Ambil digit dan titik desimal saja. Contoh: "3-479.00" jadi "3479.00".
string Hanya_Angka(const string& teks) {
    string hasil;
    for (char c : teks)
        if (isdigit(static_cast<unsigned char>(c)) || c == '.') hasil += c;
    return hasil;
}

// Potong teks supaya muat di tabel, jangan sampai terpotong di tengah karakter UTF-8.
string Potong_Teks(const string& teks, size_t maks) {
    if (teks.size() <= maks) return teks;
    size_t panjang = maks - 3;
    while (panjang > 0 && (static_cast<unsigned char>(teks[panjang]) & 0xC0) == 0x80) panjang--;
    return teks.substr(0, panjang) + "...";
}

// Baca satu angka dari keyboard, diulang sampai inputnya valid.
// Kalau input habis (EOF), program langsung berhenti.
template <typename T>
T Baca_Nilai(const string& pesan) {
    T nilai;
    cout << pesan;
    while (!(cin >> nilai)) {
        if (cin.eof()) {
            cout << endl << "Input berakhir. Program selesai." << endl;
            exit(0);
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Input tidak valid, coba lagi: ";
    }
    return nilai;
}

// Muat data produk dari CSV. Mengembalikan false kalau filenya tidak ketemu.
bool Muat_Data_Produk(vector<Produk>& katalog) {
    int jumlahData = 0;   // penghitung berapa data yang berhasil masuk
    int idOtomatis = 1001;

    // buka file CSV
    ifstream file("data/amazon_clean.csv");

    // jika file tidak ada di folder data/, coba buka di folder yang sama
    if (!file.is_open()) {
        file.open("amazon_clean.csv");
    }

    if (!file.is_open()) {
        return false;
    }

    string header;
    getline(file, header); // lewati baris pertama (judul kolom)

    // baca data baris demi baris sampai habis
    string teksHarga, teksRating, teksTerjual;

    katalog.push_back(Produk());
    while (getline(file, katalog[jumlahData].nama, ',')) {
        // baca 3 kolom sisanya dipisahkan oleh koma dan enter
        getline(file, teksHarga, ',');
        getline(file, teksTerjual, ','); // kolom ke-3: reviews_count (terjual)
        getline(file, teksRating);       // kolom ke-4: rating (diakhiri enter)

        // simpan ID dan ubah teks menjadi angka
        katalog[jumlahData].id = idOtomatis;
        katalog[jumlahData].harga = stod(teksHarga);
        katalog[jumlahData].rating = stod(teksRating);
        katalog[jumlahData].terjual = stoi(teksTerjual);
        katalog[jumlahData].Skor_Relevansi_Produk = 0.0;

        idOtomatis++;
        jumlahData++;
        katalog.push_back(Produk());
    }
    katalog.pop_back();

    file.close();

    // tes bahwa dataset berhasil dimuat
    cout << "BERHASIL MEMUAT DATASET AMAZON" << endl;
    cout << "Total produk terbaca : " << jumlahData << " produk" << endl << endl;
    return true;
}

// Opsi 1: pengurutan katalog

// Skor = (harga x bobot harga) + (rating x bobot rating) + (terjual x bobot terjual).
// Dihitung untuk semua produk sebelum pengurutan dimulai.
void Hitung_Skor_Relevansi(vector<Produk>& Katalog,
                           double Bobot_Harga, double Bobot_Rating, double Bobot_Terjual) {
    for (size_t i = 0; i < Katalog.size(); i++) {
        Katalog[i].Skor_Relevansi_Produk = (Katalog[i].harga   * Bobot_Harga)
                                         + (Katalog[i].rating  * Bobot_Rating)
                                         + (Katalog[i].terjual * Bobot_Terjual);
    }
}

// Pembanding untuk urutan menurun: skor lebih besar di depan.
// Kalau skor sama, ID yang lebih kecil dulu supaya hasilnya selalu sama.
bool Lebih_Besar(const Produk& A, const Produk& B) {
    if (A.Skor_Relevansi_Produk != B.Skor_Relevansi_Produk)
        return A.Skor_Relevansi_Produk > B.Skor_Relevansi_Produk;
    return A.id < B.id;
}

// Quick sort rekursif dengan pivot elemen tengah.
void Manual_Quick_Sort(vector<Produk>& Katalog, int Indeks_Awal_Array, int Indeks_Akhir_Array) {
    // Sub-array berisi 0 atau 1 elemen sudah terurut, jadi rekursi berhenti di situ.
    if (Indeks_Awal_Array < Indeks_Akhir_Array) {

        // pivot = elemen tengah, disalin dulu sebelum dipindah ke ujung
        int Indeks_Tengah = Indeks_Awal_Array + (Indeks_Akhir_Array - Indeks_Awal_Array) / 2;
        const Produk Nilai_Tengah_Atau_Pivot = Katalog[Indeks_Tengah];

        // Pivot dipindah ke ujung, lalu semua elemen yang "lebih besar" dari pivot
        // digeser ke kiri.
        swap(Katalog[Indeks_Tengah], Katalog[Indeks_Akhir_Array]);
        int Indeks_Pivot = Indeks_Awal_Array;
        for (int j = Indeks_Awal_Array; j < Indeks_Akhir_Array; j++) {
            if (Lebih_Besar(Katalog[j], Nilai_Tengah_Atau_Pivot)) {
                swap(Katalog[j], Katalog[Indeks_Pivot]);
                Indeks_Pivot++;
            }
        }
        swap(Katalog[Indeks_Pivot], Katalog[Indeks_Akhir_Array]); // pivot ke posisi akhirnya

        // urutkan bagian kiri dan kanan pivot
        Manual_Quick_Sort(Katalog, Indeks_Awal_Array, Indeks_Pivot - 1);
        Manual_Quick_Sort(Katalog, Indeks_Pivot + 1, Indeks_Akhir_Array);
    }
}

// Cetak produk teratas dalam bentuk tabel.
void Tampilkan_Daftar_Katalog_Produk_Terurut(const vector<Produk>& Katalog) {
    int jumlah = static_cast<int>(min<size_t>(JUMLAH_TAMPIL, Katalog.size()));

    cout << endl << "--- " << jumlah << " PRODUK TERATAS (dari " << Katalog.size()
         << " produk, urut skor relevansi menurun) ---" << endl;
    cout << left << setw(5) << "No" << setw(7) << "ID" << setw(52) << "Nama"
         << right << setw(10) << "Harga" << setw(8) << "Rating" << setw(10) << "Terjual"
         << setw(16) << "Skor" << endl;
    cout << string(108, '-') << endl;

    for (int i = 0; i < jumlah; i++) {
        cout << left << setw(5) << (i + 1) << setw(7) << Katalog[i].id
             << setw(52) << Potong_Teks(Katalog[i].nama, 50)
             << right << fixed
             << setw(10) << setprecision(2) << Katalog[i].harga
             << setw(8)  << setprecision(1) << Katalog[i].rating
             << setw(10) << Katalog[i].terjual
             << setw(16) << setprecision(2) << Katalog[i].Skor_Relevansi_Produk << endl;
    }
    cout << endl;
}

void Pengurutan_Katalog(vector<Produk>& Katalog) {
    cout << endl << "=== PENGURUTAN KATALOG ===" << endl;
    cout << "Skor = (Harga x Bobot_Harga) + (Rating x Bobot_Rating) + (Terjual x Bobot_Terjual)" << endl;
    cout << "Bobot boleh 0 atau negatif (mis. bobot harga negatif = harga murah diprioritaskan)." << endl;

    double Bobot_Harga   = Baca_Nilai<double>("Bobot Harga   : ");
    double Bobot_Rating  = Baca_Nilai<double>("Bobot Rating  : ");
    double Bobot_Terjual = Baca_Nilai<double>("Bobot Terjual : ");

    Hitung_Skor_Relevansi(Katalog, Bobot_Harga, Bobot_Rating, Bobot_Terjual);

    // urutkan seluruh katalog, dari indeks 0 sampai yang terakhir
    Manual_Quick_Sort(Katalog, 0, static_cast<int>(Katalog.size()) - 1);

    Tampilkan_Daftar_Katalog_Produk_Terurut(Katalog);
}

// Opsi 2: pencarian ID (Interpolation Search)

// Cari Target_ID di Nilai_ID. Mengembalikan indeksnya, atau -1 kalau tidak ada.
// Nilai_ID harus sudah terurut naik.
int Interpolation_Search(const vector<int>& Nilai_ID, int Target_ID) {
    int Indeks_Awal = 0;
    int Indeks_Akhir = static_cast<int>(Nilai_ID.size()) - 1;

    // Berhenti kalau rentang pencarian sudah habis atau target di luar rentang nilai.
    while (Indeks_Awal <= Indeks_Akhir &&
           Target_ID >= Nilai_ID[Indeks_Awal] &&
           Target_ID <= Nilai_ID[Indeks_Akhir]) {

        // perkiraan posisi target dengan rumus interpolasi
        int Pos;
        if (Nilai_ID[Indeks_Akhir] == Nilai_ID[Indeks_Awal]) {
            Pos = Indeks_Awal; // hindari pembagian dengan nol
        } else {
            Pos = Indeks_Awal + static_cast<int>(
                  (static_cast<long long>(Target_ID - Nilai_ID[Indeks_Awal]) *
                   (Indeks_Akhir - Indeks_Awal)) /
                  (Nilai_ID[Indeks_Akhir] - Nilai_ID[Indeks_Awal]));
        }

        if (Nilai_ID[Pos] == Target_ID) {
            return Pos;
        } else if (Nilai_ID[Pos] < Target_ID) {
            Indeks_Awal = Pos + 1;      // target ada di sebelah kanan
        } else {
            Indeks_Akhir = Pos - 1;     // target ada di sebelah kiri
        }
    }
    return -1;
}

void Tampilkan_Detail_Produk(const Produk& p) {
    cout << endl << "--- DETAIL RINCIAN PRODUK ---" << endl;
    cout << "ID Produk : " << p.id << endl;
    cout << "Nama      : " << p.nama << endl;
    cout << "Harga     : $" << fixed << setprecision(2) << p.harga << endl;
    cout << "Rating    : " << fixed << setprecision(1) << p.rating << endl;
    cout << "Terjual   : " << p.terjual << endl << endl;
}

void Pencarian_ID(vector<Produk>& Katalog) {
    cout << endl << "=== PENCARIAN PRODUK BERDASARKAN ID ===" << endl;
    int Target_ID = Baca_Nilai<int>("Masukkan ID produk yang dicari : ");

    // Interpolation search butuh data terurut naik, jadi katalog diurutkan
    // berdasarkan ID dulu. Efek sampingnya, urutan skor dari Opsi 1 ikut hilang.
    sort(Katalog.begin(), Katalog.end(),
         [](const Produk& A, const Produk& B) { return A.id < B.id; });

    vector<int> Nilai_ID;
    Nilai_ID.reserve(Katalog.size());
    for (size_t i = 0; i < Katalog.size(); i++) Nilai_ID.push_back(Katalog[i].id);

    int Pos = Interpolation_Search(Nilai_ID, Target_ID);

    if (Pos != -1) {
        Tampilkan_Detail_Produk(Katalog[Pos]);
    } else {
        cout << endl << "Data Produk Tidak Ditemukan" << endl << endl;
    }
}

// Menu utama
void Tampilkan_Menu_Utama() {
    cout << "========================================" << endl;
    cout << "   E-COMMERCE DYNAMIC FILTER ENGINE" << endl;
    cout << "========================================" << endl;
    cout << " 1. Pengurutan Katalog" << endl;
    cout << " 2. Pencarian ID Produk" << endl;
    cout << " 3. Keluar" << endl;
    cout << "========================================" << endl;
}

int main() {
    vector<Produk> Katalog;

    if (!Muat_Data_Produk(Katalog)) {
        cout << "File amazon_clean.csv tidak ditemukan!" << endl;
        return 0;
    }

    int Pilihan = 0;
    do {
        Tampilkan_Menu_Utama();
        Pilihan = Baca_Nilai<int>("Pilih Opsi (1-3) : ");

        if (Pilihan == 1) {
            Pengurutan_Katalog(Katalog);
        } else if (Pilihan == 2) {
            Pencarian_ID(Katalog);
        } else if (Pilihan == 3) {
            cout << endl << "Terima kasih. Program selesai." << endl;
        } else {
            cout << endl << "Opsi tidak valid, pilih 1 sampai 3." << endl << endl;
        }
    } while (Pilihan != 3);

    return 0;
}