#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <functional>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using namespace std::chrono;

typedef std::function<void(FILE*, const char *, size_t)>  Writer;
typedef std::function<void(FILE*)> Defer;


long timer_with_microseconds(std::function<void()> callable) {
    auto begin = high_resolution_clock::now();

    callable();

    auto end = high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    return duration.count();
}

void print_help_infos(std::string name, long cost, int times, size_t size) {
    size_t total_bytes_written = size * times;
    double seconds = cost / 1000000.f;
    double ops_per_sec = times / seconds;
    double bytes_per_sec = total_bytes_written / seconds;

    cout << name << "[times: " << times << ", bytes: " << size << "]";
    cout << ",\tcost: " << std::fixed << seconds << " seconds,\t";
    cout << std::fixed << ops_per_sec << " ops/sec,\t";
    cout << std::fixed << bytes_per_sec << " bytes/sec" << endl;
}

void benchmark(std::string name, int times, size_t size, Writer writer, Defer defer) {  
    assert(times > 0 && size > 0);

    const char *tmp_file_name = "/mnt/e/tmp.txt";
    const int run_times = 10;
    long microseconds = 0;
    for (int i = 0; i < run_times; i++) {
        FILE *fp;
        if ((fp = fopen(tmp_file_name, "wb+")) == NULL) {
            cout << "open file failed" << endl;
            exit(-1);
        }

        microseconds += timer_with_microseconds([&]() {
            char bytes[size];
            for (int j = 0; j < times; j++) {
                writer(fp, bytes, size);
            }
            defer(fp);
        });

        fclose(fp);
        remove(tmp_file_name);
    }

    print_help_infos(name, microseconds, times * run_times, size);
}

void buffered_write_with_flush(FILE *fp, const char *bytes, size_t size) {
    if (!fwrite(bytes, size, 1, fp)) {
        cout << "write file failed" << endl;
        exit(-1);
    }
    fflush(fp);
}

void buffered_write_with_fsync(FILE *fp, const char *bytes, size_t size) {
    if (!fwrite(bytes, size, 1, fp)) {
        cout << "write file failed" << endl;
        exit(-1);
    }
    fflush(fp);
    fsync(fileno(fp));
}

void buffered_write_without_flush(FILE *fp, const char *bytes, size_t size) {
    if (!fwrite(bytes, size, 1, fp)) {
        cout << "wirte file failed" << endl;
        exit(-1);
    }
}

void empty(FILE *fp) {
    /* just ignore */
}

void flush(FILE *fp) {
    fflush(fp);
    fsync(fileno(fp));
}

void _fsync(FILE *fp) {
    fsync(fileno(fp));
}

int main() {
    const size_t B = 1;
    const size_t KB = B * 1024;
    const size_t KB_10 = KB * 10;
    const size_t MB = KB * 1024;
    const size_t MB_5 = MB * 5;

    benchmark("buffered_write_with_flush", 1000, B, buffered_write_with_flush, _fsync);
    benchmark("buffered_write_with_fsync", 1000, B, buffered_write_with_fsync, empty);
    benchmark("buffered_write_without_flush", 1000, B, buffered_write_without_flush, flush);
    cout << endl;

    benchmark("buffered_write_with_flush", 100, KB, buffered_write_with_flush, _fsync);
    benchmark("buffered_write_with_fsync", 100, KB, buffered_write_with_fsync, empty);
    benchmark("buffered_write_without_flush", 100, KB, buffered_write_without_flush, flush);
    cout << endl;

    benchmark("buffered_write_with_flush", 100, KB_10, buffered_write_with_flush, _fsync);
    benchmark("buffered_write_with_fsync", 100, KB_10, buffered_write_with_fsync, empty);
    benchmark("buffered_write_without_flush", 100, KB_10, buffered_write_without_flush, flush);
    cout << endl;

    benchmark("buffered_write_with_flush", 100, MB, buffered_write_with_flush, _fsync);
    benchmark("buffered_write_with_fsync", 100, MB, buffered_write_with_fsync, empty);
    benchmark("buffered_write_without_flush", 100, MB, buffered_write_without_flush, flush);
    cout << endl;

    benchmark("buffered_write_with_flush", 100, MB_5, buffered_write_with_flush, _fsync);
    benchmark("buffered_write_with_fsync", 100, MB_5, buffered_write_with_fsync, empty);
    benchmark("buffered_write_without_flush", 100, MB_5, buffered_write_without_flush, flush);
    cout << endl;

    return 0;
}
