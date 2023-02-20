/*
 * Copyright (c) 2023 CYBERDECK Studios. (MIT License)
 * https://github.com/cyberdeck-studios/WalletConnectV2_Demo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 */
#include "Cryptography.h"


#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#include "openssl/rand.h"
#include "openssl/kdf.h"
#include "openssl/sha.h"
#include "openssl/md5.h"
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/err.h"
#include "openssl/aes.h"
THIRD_PARTY_INCLUDES_END
#undef UI

bool Cryptography::GenerateEd25519(FString& Public, FString& Private)
{
	EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, nullptr);
	if (!ctx)
	{
		return false;
	}
	if (EVP_PKEY_keygen_init(ctx) <= 0)
	{
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	EVP_PKEY* pkey = nullptr;
	if (EVP_PKEY_keygen(ctx, &pkey) <= 0)
	{
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	std::size_t private_key_len, public_key_len;
	if (EVP_PKEY_get_raw_private_key(pkey, nullptr, &private_key_len) <= 0)
	{
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	if (EVP_PKEY_get_raw_public_key(pkey, nullptr, &public_key_len) <= 0)
	{
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}

	unsigned char* private_key = new unsigned char[private_key_len];
	unsigned char* public_key = new unsigned char[public_key_len];

	if (EVP_PKEY_get_raw_private_key(pkey, private_key, &private_key_len) <= 0)
	{
		delete[] private_key;
		delete[] public_key;
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	if (EVP_PKEY_get_raw_public_key(pkey, public_key, &public_key_len) <= 0)
	{
		delete[] private_key;
		delete[] public_key;
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}

	Public = BytesToHex(public_key, public_key_len).ToLower();
	Private = BytesToHex(private_key, private_key_len).ToLower();
	delete[] private_key;
	delete[] public_key;
	EVP_PKEY_free(pkey);
	EVP_PKEY_CTX_free(ctx);
	return true;
}

bool Cryptography::GenerateX25519(FString& Public, FString& Private)
{
	EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_X25519, nullptr);
	if (!ctx)
	{
		return false;
	}
	if (EVP_PKEY_keygen_init(ctx) <= 0)
	{
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	EVP_PKEY* pkey = nullptr;
	if (EVP_PKEY_keygen(ctx, &pkey) <= 0)
	{
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	std::size_t private_key_len, public_key_len;
	if (EVP_PKEY_get_raw_private_key(pkey, nullptr, &private_key_len) <= 0)
	{
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	if (EVP_PKEY_get_raw_public_key(pkey, nullptr, &public_key_len) <= 0)
	{
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}

	unsigned char* private_key = new unsigned char[private_key_len];
	unsigned char* public_key = new unsigned char[public_key_len];

	if (EVP_PKEY_get_raw_private_key(pkey, private_key, &private_key_len) <= 0)
	{
		delete[] private_key;
		delete[] public_key;
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	if (EVP_PKEY_get_raw_public_key(pkey, public_key, &public_key_len) <= 0)
	{
		delete[] private_key;
		delete[] public_key;
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}

	Public = BytesToHex(public_key, public_key_len).ToLower();
	Private = BytesToHex(private_key, private_key_len).ToLower();
	delete[] private_key;
	delete[] public_key;
	EVP_PKEY_free(pkey);
	EVP_PKEY_CTX_free(ctx);
	return true;
}

bool Cryptography::Digest(FString Data, FString& Hash)
{
	unsigned char* data = new unsigned char[Data.Len()/2];
	HexToBytes(Data, data);
	EVP_MD_CTX* ctx = EVP_MD_CTX_create();
	const EVP_MD* evp = EVP_sha256();
	if (!EVP_DigestInit(ctx, evp))
	{
		delete[] data;
		EVP_MD_CTX_free(ctx);
		return false;
	}
	if (!EVP_DigestUpdate(ctx, data, Data.Len() / 2))
	{
		delete[] data;
		EVP_MD_CTX_free(ctx);
		return false;
	}

	unsigned int length = 0;
	unsigned char* HashBytes = new unsigned char[EVP_MAX_MD_SIZE];
	if (!EVP_DigestFinal(ctx, HashBytes, &length))
	{
		delete[] data;
		delete[] HashBytes;
		EVP_MD_CTX_free(ctx);
		return false;
	}
	Hash = BytesToHexLower(HashBytes, length);
	delete[] data;
	delete[] HashBytes;
	EVP_MD_CTX_free(ctx);
	return true;
}

bool Cryptography::Sign(FString Data, FString Private, FString& Signature)
{
	const std::string msg = std::string(TCHAR_TO_UTF8(*Data));

	const unsigned char* data = (unsigned char*)msg.c_str();
	size_t Size = msg.length();

	unsigned char* pvkey = new unsigned char[Private.Len() / 2];
	HexToBytes(Private, pvkey);
	EVP_PKEY* PKey = EVP_PKEY_new_raw_private_key(EVP_PKEY_ED25519, nullptr, pvkey, 32);
	delete[] pvkey;
	size_t signature_len = 0;
	EVP_MD_CTX* SignCtx = EVP_MD_CTX_new();

	if (!EVP_DigestSignInit(SignCtx, nullptr, nullptr, nullptr, PKey))
	{
		EVP_PKEY_free(PKey);
		EVP_MD_CTX_free(SignCtx);
		return false;
	}

	if (!EVP_DigestSign(SignCtx, nullptr, &signature_len, data, Size))
	{
		EVP_PKEY_free(PKey);
		EVP_MD_CTX_free(SignCtx);
		return false;
	}

	unsigned char* signature = new unsigned char[signature_len];

	if (!EVP_DigestSign(SignCtx, signature, &signature_len, data, Size))
	{
		EVP_PKEY_free(PKey);
		EVP_MD_CTX_free(SignCtx);
		delete[] signature;
		return false;
	}
	
	Signature = FBase64::Encode(signature, signature_len);
	
	Signature = Signature.Replace(TEXT("\n"), TEXT(""));
	Signature = Signature.Replace(TEXT("="), TEXT(""));
	Signature = Signature.Replace(TEXT("+"), TEXT("-"));
	Signature = Signature.Replace(TEXT("/"), TEXT("_"));
	delete[] signature;
	EVP_PKEY_free(PKey);
	EVP_MD_CTX_free(SignCtx);
	return true;

}

bool Cryptography::Exchangekeys(FString My_PrivateKey, FString Peer_PublicKey, FString& SharedKey)
{
	unsigned char* my_private_key = new unsigned char[32];
	HexToBytes(My_PrivateKey, my_private_key);
	EVP_PKEY* private_key = EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, nullptr, my_private_key, 32);

	unsigned char* peer_public_key = new unsigned char[32];
	HexToBytes(Peer_PublicKey, peer_public_key);
	EVP_PKEY* peerkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, nullptr, peer_public_key, 32);

	delete[] my_private_key;
	delete[] peer_public_key;

	EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new(private_key, NULL);

	if (!EVP_PKEY_derive_init(pctx))
	{
		EVP_PKEY_free(private_key);
		EVP_PKEY_free(peerkey);
		EVP_PKEY_CTX_free(pctx);
		return false;
	}

	if (!EVP_PKEY_derive_set_peer(pctx, peerkey))
	{
		EVP_PKEY_free(private_key);
		EVP_PKEY_free(peerkey);
		EVP_PKEY_CTX_free(pctx);
		return false;
	}

	EVP_PKEY_free(private_key);
	EVP_PKEY_free(peerkey);
	size_t shared_key_len;

	if (!EVP_PKEY_derive(pctx, NULL, &shared_key_len))
	{
		EVP_PKEY_CTX_free(pctx);
		return false;
	}

	unsigned char* shared_key = new unsigned char[shared_key_len];

	if (!EVP_PKEY_derive(pctx, shared_key, &shared_key_len))
	{
		EVP_PKEY_CTX_free(pctx);
		delete[] shared_key;
		return false;
	}

	EVP_PKEY_CTX_free(pctx);

	EVP_PKEY_CTX* kdf_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);

	if (!EVP_PKEY_derive_init(kdf_ctx))
	{
		EVP_PKEY_CTX_free(kdf_ctx);
		delete[] shared_key;
		return false;
	}

	if (!EVP_PKEY_CTX_set_hkdf_md(kdf_ctx, EVP_sha256()))
	{
		EVP_PKEY_CTX_free(kdf_ctx);
		delete[] shared_key;
		return false;
	}

	if (!EVP_PKEY_CTX_set1_hkdf_salt(kdf_ctx, NULL, 0))
	{
		EVP_PKEY_CTX_free(kdf_ctx);
		delete[] shared_key;
		return false;
	}

	if (!EVP_PKEY_CTX_set1_hkdf_key(kdf_ctx, shared_key, shared_key_len))
	{
		EVP_PKEY_CTX_free(kdf_ctx);
		delete[] shared_key;
		return false;
	}

	unsigned char derived_key[32];
	size_t derived_key_len = 32;
	if (!EVP_PKEY_derive(kdf_ctx, derived_key, &derived_key_len))
	{
		EVP_PKEY_CTX_free(kdf_ctx);
		delete[] shared_key;
		return false;
	}

	EVP_PKEY_CTX_free(kdf_ctx);
	delete[] shared_key;
	SharedKey = BytesToHexLower(derived_key, derived_key_len);
	return true;
}

bool Cryptography::RandomKey(FString &RandomKey)
{
	unsigned char buffer[32];
	RAND_bytes(buffer, 32);
	RandomKey = BytesToHexLower(buffer, 32);
	return true;
}

bool Cryptography::Encrypt(FString Message, FString SharedKey, FString& EncryptedMessage)
{
	std::string message = std::string(TCHAR_TO_UTF8(*Message));
	int outlen;
	int ciphertext_len = message.length() + EVP_CIPHER_block_size(EVP_chacha20_poly1305());
	unsigned char* ciphertext = new unsigned char[ciphertext_len];

	unsigned char nonce[12];
	unsigned char key[32];
	unsigned char tag[16];

	RAND_bytes(nonce, 12);
	HexToBytes(SharedKey, key);

	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!EVP_EncryptInit_ex(ctx, EVP_chacha20_poly1305(), nullptr, key, nonce))
	{
		EVP_CIPHER_CTX_free(ctx);
		delete[] ciphertext;
		return false;
	}
	if (!EVP_EncryptUpdate(ctx, ciphertext, &outlen, (unsigned char*)message.c_str(), message.length()))
	{
		EVP_CIPHER_CTX_free(ctx);
		delete[] ciphertext;
		return false;
	}
	int tmp = 0;
	if (!EVP_EncryptFinal_ex(ctx, ciphertext + outlen, &tmp))
	{
		EVP_CIPHER_CTX_free(ctx);
		delete[] ciphertext;
		return false;
	}
	if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, 16, tag))
	{
		EVP_CIPHER_CTX_free(ctx);
		delete[] ciphertext;
		return false;
	}
	EVP_CIPHER_CTX_free(ctx);
	outlen += tmp;
	unsigned char* result = new unsigned char[16 + 13 + outlen];
	result[0] = 0;
	memcpy(result + 1, nonce, 12);
	memcpy(result + 1 + 12, ciphertext, outlen);
	memcpy(result + 1 + 12 + outlen, tag, 16);
	EncryptedMessage = FBase64::Encode(result, 1 + 12 + outlen + 16);
	delete[] ciphertext;
	delete[] result;
	return true;
}

bool Cryptography::Decrypt(FString EncryptedMessage, FString SharedKey, FString &Message)
{
	int ciphertext_len = FBase64::GetDecodedDataSize(EncryptedMessage);
	unsigned char* ciphertext = new unsigned char[ciphertext_len];
	unsigned char* result = new unsigned char[ciphertext_len];
	FBase64::Decode(*EncryptedMessage, EncryptedMessage.Len(), ciphertext);
	unsigned char nonce[12];
	unsigned char key[32];
	unsigned char tag[16];
	memcpy(nonce, ciphertext + 1, 12);
	memcpy(result, ciphertext + 1 + 12, ciphertext_len - 1 - 12 - 16);
	memcpy(tag, ciphertext + ciphertext_len - 16, 16);
	delete[] ciphertext;
	HexToBytes(SharedKey, key);
	int outlen;
	int plaintext_len = ciphertext_len - 1 - 12 - 16;
	unsigned char* plaintext = new unsigned char[plaintext_len];
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!EVP_DecryptInit_ex(ctx, EVP_chacha20_poly1305(), nullptr, key, nonce))
	{
		EVP_CIPHER_CTX_free(ctx);
		delete[] result;
		delete[] plaintext;
		return false;
	}
	if (!EVP_DecryptUpdate(ctx, plaintext, &outlen, result, plaintext_len))
	{
		EVP_CIPHER_CTX_free(ctx);
		delete[] result;
		delete[] plaintext;
		return false;
	}
	int tmp = 0;
	if (!EVP_DecryptFinal_ex(ctx, plaintext + outlen, &tmp))
	{
		EVP_CIPHER_CTX_free(ctx);
		delete[] result;
		delete[] plaintext;
		return false;
	}
	outlen += tmp;
	EVP_CIPHER_CTX_free(ctx);
	delete[] result;
	FString decrypted_hex = BytesToHexLower(plaintext, plaintext_len);
	TArray<uint8> Bytes;
	Bytes.SetNum(decrypted_hex.Len() / 2);
	int32 Total = HexToBytes(decrypted_hex, Bytes.GetData());
	Bytes.SetNum(Total);

	if (Bytes.Last() != 0)
	{
		TArray<uint8> Copy(Bytes);
		Copy.Add(0);
		Message = FString(UTF8_TO_TCHAR(Copy.GetData()));
		return true;
	}
	Message = FString(UTF8_TO_TCHAR(Bytes.GetData()));
	return true;
}
