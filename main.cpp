/*
 * WAV_Reader.cpp
 *
 *  Created on: 23 Feb 2017
 *      Author: ML15AAF
 */

#include <WAV_Reader.h>

WAV_Reader::Config::Config() {
    format_tag = 0;
    channels = 0;
    samples_per_sec = 0;
    avg_bytes_per_sec = 0;
    block_align = 0;
    bits_per_sample = 0;
    data_pos = 0;
    data_length = 0;
    file_size = 0;
}

WAV_Reader::WAV_Reader() :
        fpp(NULL), _current_pos(0), _loop(false) {

}

int WAV_Reader::open(FILE **filepp) {
    int success = 0;

    fpp = filepp;

    if (*filepp != NULL) {
        if (check_bytes(*fpp, "RIFF", 4)) {
            if (check_bytes(*fpp, "WAVE", 4, 8)) {
                config.file_size = read_num(*fpp, 4, 4);

                int chunk_pos = find_chunk(*fpp, "fmt ", config.file_size, 12);

                if (chunk_pos != -1) {
                    config.format_tag = read_num(*fpp, 2, chunk_pos + 8);
                    config.channels = read_num(*fpp, 2, chunk_pos + 10);
                    config.samples_per_sec = read_num(*fpp, 4, chunk_pos + 12);
                    config.avg_bytes_per_sec = read_num(*fpp, 4, chunk_pos + 16);
                    config.block_align = read_num(*fpp, 2, chunk_pos + 20);
                    config.bits_per_sample = read_num(*fpp, 2, chunk_pos + 22);
                } else success = WAV_READER_FO_NO_FMT;

                chunk_pos = find_chunk(*fpp, "data", config.file_size, 12);

                if (chunk_pos != -1) {
                    config.data_length = read_num(*fpp, 4, chunk_pos + 4);
                    config.data_pos = chunk_pos + 8;
                    reset();

                } else success = success + WAV_READER_FO_NO_DATA;

                if (success == 0) success = config.data_length;

            } else success = WAV_READER_FO_NOT_WAV;
        } else success = WAV_READER_FO_NOT_RIFF;
    } else success = WAV_READER_FO_NULL_PTR;



    return success;
}

int WAV_Reader::read(int buff[], int size, int len) { // buffer is what we will access to perform our fft
    int data_left = config.data_length - _current_pos;
    int to_read = len*size;

    if (to_read > data_left) {
        if (data_left == 0 && _loop) {
            reset();
        } else {
            to_read = data_left;
        }
    }

    char* bytes = new char[to_read];
    size_t read_bytes = fread(bytes, 1, to_read, *fpp);
    read_bytes -= read_bytes % size;
    int count = 0;
    for (int i = 0; i < read_bytes; i += size){
    	buff[count++] = get_num(bytes+i, size);
    }
    delete[] bytes;
    move_read_position(read_bytes, _current_pos);

    to_read = len*size - read_bytes;
    if (to_read > 0 && _loop) {
        read_bytes += read(buff + read_bytes/size, size, to_read);
    }

    return read_bytes;
}

void WAV_Reader::reset() {
    seek(0, 0);
}

bool WAV_Reader::loop() {
    return _loop;
}

void WAV_Reader::loop(bool enable) {
    _loop = enable;
}

void WAV_Reader::seek(int num, int start) {
    if (start < 0) start = _current_pos;
    move_read_position(num, start);
    fseek(*fpp, _current_pos + config.data_pos, SEEK_SET);
}

uint16_t WAV_Reader::channels() {
    return config.channels;
}

uint32_t WAV_Reader::samples_per_sec() {
    return config.samples_per_sec;
}

uint32_t WAV_Reader::bytes_per_sec() {
	return config.avg_bytes_per_sec;
}

uint16_t WAV_Reader::block_align() {
    return config.block_align;
}

uint16_t WAV_Reader::bits_per_sample() {
    return config.bits_per_sample;
}

uint32_t WAV_Reader::data_length() {
	return config.data_length;
}

/** Private **/

int WAV_Reader::read_bytes(FILE *fp, char* out, int len_to_read, int start_off) {
    fseek(fp, start_off, SEEK_SET);

    for (int i = 0; i < len_to_read; i++) {
        out[i] = fgetc(fp);
    }

    return len_to_read;

}

bool WAV_Reader::check_bytes(FILE *fp, char* check, int len, int start) {
    bool success = false;
    char extracted[len + 1];

    read_bytes(fp, extracted, len, start);
    extracted[len] = 0;

    success = strcmp(extracted, check) == 0;

    return success;
}

uint32_t WAV_Reader::read_num(FILE *fp, int len, int start) {
    char extracted[len];
    read_bytes(fp, extracted, len, start);

    return get_num(extracted, len);
}

uint32_t WAV_Reader::get_num(char input[], int byte_count) {

    uint32_t output = 0;
    for (int i = 0; i < byte_count; i++) {
        // numbers are stored least significant byte. So reverse and add to find the number
        output += input[i] << (i * 8);
    }

    return output;

}

int WAV_Reader::find_chunk(FILE *fp, char* check, int max_len, int start) {
    int result = -1;

    int pos = start;

    while ((pos + 8) < max_len && result == -1) {
        if (check_bytes(fp, check, 4, pos)) result = pos;
        else pos += read_num(fp, 4, pos + 4) + 8;
    }

    return result;

}

int WAV_Reader::move_read_position(int count, int start) {
    _current_pos = start + count;
    if (_current_pos > config.data_length) {
        if (loop()) _current_pos %= config.data_length;
        else _current_pos = config.data_length;
    }

    return _current_pos;
}
