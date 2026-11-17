# E-Commerce Dynamic Filter Engine

Aplikasi simulasi backend sistem penyaringan (filter), pengurutan (sorting), dan pencarian (searching) katalog produk e-commerce berbasis **Command Line Interface (CLI)** menggunakan bahasa pemrograman **C++**.

Proyek ini dikembangkan untuk mengatasi keterbatasan fitur pengurutan dimensi tunggal pada platform marketplace tradisional (yang hanya mengurutkan berdasarkan harga termurah atau rating tertinggi saja) dengan menghadirkan sistem **Weighted Relevance Sorting** yang dinamis sesuai prioritas pengguna.

---

## Anggota Kelompok — Penguras Toyagama
**Fakultas Matematika dan Ilmu Pengetahuan Alam — Universitas Gadjah Mada (2026)**

               Nama Lengkap       |           NIM         |
1. **Sayidah Afifah Karin**       | `26/576971/PA/24120`  |
2.  **Kiara Aisha Putri**         | `26/577981/PA/24208`  |
3.  **Audina Dwiyunita Putri**    | `26/57428/PA/24748`   |
4.  **Muhammad Ilham Dhiyaulhaq** | `26/577829/PA/24195`  |

---


## Fitur Utama Sistem

1. **Input Bobot Prioritas Dinamis**
   Pengguna dapat menentukan sendiri bobot preferensi untuk tiga kriteria utama produk: **Harga**, **Rating**, dan **Jumlah Terjual**.

2. **Pengurutan Katalog Produk (Weighted Relevance Sorting)**
   Menghitung **Skor Relevansi** untuk setiap produk menggunakan rumus terbobot, kemudian mengurutkan katalog secara descending (tertinggi ke terendah) menggunakan algoritma **Manual Quick Sort & Custom Comparator**:
   ```text
   Skor_Relevansi = (Harga * Bobot_Harga) + (Rating * Bobot_Rating) + (Terjual * Bobot_Terjual)

3. **Pencarian Produk Spesifik berdasarkan ID**
    Melakukan pra-pengurutan ID secara ascending menggunakan std::sort (<algorithm>), lalu mencari entitas produk secara presisi menggunakan algoritma Interpolation Search dengan rumus estimasi posisi:
    Pos = Indeks_Awal + ((Target_ID - Nilai_ID[Indeks_Awal]) / (Nilai_ID[Indeks_Akhir] - Nilai_ID[Indeks_Awal])) * (Indeks_Akhir - Indeks_Awal)

---


## Dataset & Pra-pemrosesan (Data Cleaning)
Sumber Dataset: Publik dataset Amazon (amazon-products.csv) dari repositori luminati-io/eCommerce-dataset-samples (1.000 data awal, 55 atribut).

**Pra-pemrosesan (Preprocessing):**
1. Seleksi Fitur: Memangkas 51 kolom yang tidak relevan dan mengambil 4 atribut utama (title, final_price, 
   rating,reviews_count).

2. Standardisasi Mata Uang: Mengeliminasi 33 produk bermata uang non-USD (INR dan GBP) sehingga tersisa 967 
   produk bersih (100% USD).

3. Auto-Indexing ID: Menghasilkan ID_Produk numerik berurutan (1001–1967) saat tahap Muat Data Produk agar 
   dapat diproses secara matematis oleh algoritma Interpolation Search.

---


## Alur Kerja Program (Flowchart Overview)
1. **Mulai & Muat Data Produk:** Program membaca dataset CSV bersih ke dalam memori (vector<Produk>).

2. **Menu Utama (Pilih Opsi 1–3):**
    - (opsi 1) Pengurutan Katalog: Input bobot prioritas -> Hitung Skor Relevansi -> Urutkan dengan Manual 
      Quick Sort -> Tampilkan daftar katalog terurut -> Kembali ke menu utama.

    - (opsi 2) Pencarian ID: Input Target_ID -> Urutkan ID dengan std::sort -> Cari dengan Interpolation 
      Search -> Tampilkan detail produk (atau pesan Data Produk Tidak Ditemukan) -> Kembali ke menu utama.

    - (opsi 3) Keluar: Mengakhiri eksekusi program.

---
