package main

import (
	"bufio"
	"fmt"
	"io"
	"os"
)

func usage() string {
	return `simulator <model_file:path> <trace_file:path>`
}

func main() {
	if len(os.Args) != 3 {
		fmt.Fprintf(os.Stderr, usage())
		os.Exit(1)
	}

	// read model
	model, err := LoadModel(os.Args[1])
	if err != nil {
		panic(err)
	}

	// read trace
	traceFile, err := os.Open(os.Args[2])
	if err != nil {
		panic(err)
	}
	traceReader := bufio.NewReader(traceFile)

	state := newState(model.resolution)
	commands := 0
	for {
		commands += len(state.bots)

		fmt.Fprintf(os.Stderr, "t=%d energy=%d\n", commands, state.energy)

		lines := make([]string, 0)

		for i := 0; i < len(state.bots); i++ {
			line, _, err := traceReader.ReadLine()
			if err == io.EOF {
				break
			}

			lines = append(lines, string(line))
		}

		err = state.update(lines)
		if err != nil {
			panic(fmt.Errorf("commands#%d, line=%v, %v", commands, lines, err))
		}

		if len(state.bots) == 0 {
			break
		}
	}

	/*

		for i := 0; i < model.resolution; i++ {
			for j := 0; j < model.resolution; j++ {
				for k := 0; k < model.resolution; k++ {
					if model.matrix[i][j][k] != state.model.matrix[i][j][k] {
						panic(fmt.Errorf("model mismatch at (%d,%d,%d) (expected = %v, but got %v)",
							k, j, i, model.matrix[i][j][k], state.model.matrix[i][j][k]))
					}
				}
			}
		}

	*/

	fmt.Printf("%d\n", state.energy)
}
