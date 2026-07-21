#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <openssl/evp.h>
#include <openssl/rand.h>

#define AES_KEY_SIZE 32   
#define AES_IV_SIZE  16   


void secure_memzero(void *v, size_t n) {
    volatile unsigned char *p = (volatile unsigned char *)v;
    while (n--) {
        *p++ = 0;
    }
}


void modify_content(unsigned char *data, long size) {
    for (long i = 0; i < size / 2; i++) {
        unsigned char tmp = data[i];
        data[i] = data[size - 1 - i];
        data[size - 1 - i] = tmp;
    }
}


int encrypt_buffer(const unsigned char *plaintext, int plaintext_len,
                   const unsigned char *key, const unsigned char *iv,
                   unsigned char **ciphertext, int *ciphertext_len) {

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        return 0;

    int len;
    int out_len;

    *ciphertext = (unsigned char *)malloc(plaintext_len + AES_BLOCK_SIZE);
    if (!*ciphertext) {
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(*ciphertext);
        return 0;
    }

    if (EVP_EncryptUpdate(ctx, *ciphertext, &len, plaintext, plaintext_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(*ciphertext);
        return 0;
    }
    out_len = len;

    if (EVP_EncryptFinal_ex(ctx, *ciphertext + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(*ciphertext);
        return 0;
    }
    out_len += len;

    *ciphertext_len = out_len;
    EVP_CIPHER_CTX_free(ctx);
    return 1;
}


int secure_delete(const char *filename) {
    FILE *f = fopen(filename, "r+b");
    if (!f) {
        perror("open");
        return 0;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        perror("fseek");
        fclose(f);
        return 0;
    }
    long size = ftell(f);
    if (size < 0) {
        perror("ftell");
        fclose(f);
        return 0;
    }
    rewind(f);

    unsigned char buf[4096];

    for (int pass = 0; pass < 3; pass++) {
        rewind(f);
        for (long written = 0; written < size; ) {
            size_t chunk = sizeof(buf);
            if (size - written < (long)chunk)
                chunk = (size_t)(size - written);

            if (RAND_bytes(buf, (int)chunk) != 1) {
                
                for (size_t i = 0; i < chunk; i++) {
                    buf[i] = rand() & 0xFF;
                }
            }

            if (fwrite(buf, 1, chunk, f) != chunk) {
                perror("fwrite");
                fclose(f);
                return 0;
            }
            written += chunk;
        }
        fflush(f);
    }

    fclose(f);

    if (remove(filename) != 0) {
        perror("remove");
        return 0;
    }

    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("fopen");
        return 1;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        perror("fseek");
        fclose(file);
        return 1;
    }
    long size = ftell(file);
    if (size < 0) {
        perror("ftell");
        fclose(file);
        return 1;
    }
    rewind(file);

    unsigned char *data = (unsigned char *)malloc(size);
    if (!data) {
        printf("Malloc failed\n");
        fclose(file);
        return 1;
    }

    if (fread(data, 1, size, file) != (size_t)size) {
        perror("fread");
        free(data);
        fclose(file);
        return 1;
    }
    fclose(file);

    
    modify_content(data, size);

    
    unsigned char key[AES_KEY_SIZE];
    unsigned char iv[AES_IV_SIZE];

    if (RAND_bytes(key, sizeof(key)) != 1) {
        printf("RAND_bytes for key failed\n");
        free(data);
        return 1;
    }
    if (RAND_bytes(iv, sizeof(iv)) != 1) {
        printf("RAND_bytes for iv failed\n");
        free(data);
        secure_memzero(key, sizeof(key));
        return 1;
    }

    
    unsigned char *ciphertext = NULL;
    int ciphertext_len = 0;

    if (!encrypt_buffer(data, (int)size, key, iv, &ciphertext, &ciphertext_len)) {
        printf("Encryption failed\n");
        free(data);
        secure_memzero(key, sizeof(key));
        secure_memzero(iv, sizeof(iv));
        return 1;
    }

    
    file = fopen(filename, "wb");
    if (!file) {
        perror("fopen for write");
        free(data);
        free(ciphertext);
        secure_memzero(key, sizeof(key));
        secure_memzero(iv, sizeof(iv));
        return 1;
    }

    
    if (fwrite(ciphertext, 1, ciphertext_len, file) != (size_t)ciphertext_len) {
        perror("fwrite ciphertext");
        fclose(file);
        free(data);
        free(ciphertext);
        secure_memzero(key, sizeof(key));
        secure_memzero(iv, sizeof(iv));
        return 1;
    }
    fclose(file);

    
    secure_memzero(key, sizeof(key));
    secure_memzero(iv, sizeof(iv));
    secure_memzero(data, (size_t)size);
    secure_memzero(ciphertext, (size_t)ciphertext_len);

    free(data);
    free(ciphertext);

    
    if (!secure_delete(filename)) {
        printf("Secure delete failed (file may still exist!)\n");
        return 1;
    }

    printf("File modified, encrypted, and securely deleted.\n");
    return 0;
}