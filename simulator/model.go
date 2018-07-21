package main

import (
	"bufio"
	"io"
	"os"
)

type Model struct {
	resolution int
	matrix     [][][]bool
}

func newModel(r int) Model {
	m := make([][][]bool, r)
	for i := 0; i < r; i++ {
		m[i] = make([][]bool, r)
		for j := 0; j < r; j++ {
			m[i][j] = make([]bool, r)
		}
	}
	return Model{
		resolution: r,
		matrix:     m,
	}
}

func LoadModel(modelFile string) (Model, error) {
	m := Model{}

	f, err := os.Open(modelFile)
	if err != nil {
		return m, err
	}
	defer f.Close()

	r := bufio.NewReader(f)

	b, err := r.ReadByte()
	if err != nil {
		return m, err
	}

	m = newModel(int(b))

	x := 0
	y := 0
	z := 0

	for {
		b, err = r.ReadByte()
		if err == io.EOF {
			break
		}

		for i := 0; i < 8; i++ {
			if z >= m.resolution {
				break
			}

			if b&(1<<uint(i)) != 0 {
				m.matrix[z][y][x] = true
			}

			x += 1
			if x >= m.resolution {
				x = 0
				y += 1
				if y >= m.resolution {
					y = 0
					z += 1
					if z >= m.resolution {
						break
					}
				}
			}
		}
	}

	return m, nil
}

func (m *Model) DumpFile(fileName string) error {
	f, err := os.OpenFile(fileName, os.O_RDWR|os.O_CREATE, 0644)
	if err != nil {
		return err
	}
	defer f.Close()

	w := bufio.NewWriter(f)

	w.WriteByte(byte(m.resolution))

	var b byte
	var s uint

	for i := 0; i < m.resolution; i++ {
		for j := 0; j < m.resolution; j++ {
			for k := 0; k < m.resolution; k++ {
				if m.matrix[i][j][k] {
					b = b | (1 << s)
				}
				if s == 7 {
					w.WriteByte(b)
					b = 0
					s = 0
				} else {
					s += 1
				}
			}
		}
	}

	w.Flush()

	return nil
}
