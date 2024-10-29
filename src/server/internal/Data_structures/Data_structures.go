package datastructures

// Cryptography:

type AES struct {
	Key []byte
	IV  []byte
}

type AES_Input struct {
	Data      []byte
	Timestamp int64
	Filename  string
	ID        string
}
