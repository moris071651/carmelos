package cryptography

import (
	"crypto/sha256"

	datastructures "carmelos/src/server/internal/Data_structures"
)

// global variable

var AES datastructures.AES

func Hash(data []byte) []byte {
	hash := sha256.New()
	hash.Write(data)
	return hash.Sum(nil)
}

func AES_Init(input datastructures.AES_Input) {
	AES = datastructures.AES{
		Key: Hash([]byte(input.ID)),
		IV:  Hash([]byte(input.Filename)),
	}
}

func AES_Encrypt(data []byte) []byte {
	return nil
}
