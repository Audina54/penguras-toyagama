#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Produk {
    int id;
    string nama;
    double harga;
    double rating;
    int terjual;
};

int main() {
    Produk katalog[1000]; // array untuk menampung maksimal 1000 produk
    int jumlahData = 0;   // penghitung berapa data yang berhasil masuk
    int idOtomatis = 1001;

    // buka file CSV
    ifstream file("data/amazon_clean.csv");

    // jika file tidak ada di folder data/, coba buka di folder yang sama
    if (!file.is_open()) {
        file.open("amazon_clean.csv");
    }

    if (!file.is_open()) {
        cout << "File amazon_clean.csv tidak ditemukan!" << endl;
        return 0;
    }

    string header;
    getline(file, header); // lewati baris pertama (judul kolom)

    // baca data baris demi baris sampai habis
    string teksHarga, teksRating, teksTerjual;

    while (getline(file, katalog[jumlahData].nama, ',')) {
        // baca 3 kolom sisanya dipisahkan oleh koma dan enter
        getline(file, teksHarga, ',');
        getline(file, teksRating, ',');
        getline(file, teksTerjual); // kolom terakhir diakhiri baris baru (enter)

        // simpan ID dan ubah teks menjadi angka
        katalog[jumlahData].id = idOtomatis;
        katalog[jumlahData].harga = stod(teksHarga);
        katalog[jumlahData].rating = stod(teksRating);
        katalog[jumlahData].terjual = stoi(teksTerjual);

        idOtomatis++;
        jumlahData++;
    }

    file.close();

    // tes bahwa dataset berhasil dimuat
    cout << "BERHASIL MEMUAT DATASET AMAZON" << endl;
    cout << "Total produk terbaca : " << jumlahData << " produk" << endl << endl;

    cout << "--- 5 DATA PRODUK PERTAMA ---" << endl;
    for (int i = 0; i < 5; i++) {
        cout << "ID Produk : " << katalog[i].id << endl;
        cout << "Nama      : " << katalog[i].nama << endl;
        cout << "Harga     : $" << katalog[i].harga << endl;
        cout << "Rating    : " << katalog[i].rating << endl;
        cout << "Terjual   : " << katalog[i].terjual << endl << endl ;
    }

    return 0;
}