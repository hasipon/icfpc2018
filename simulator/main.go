package main

import (
	"bufio"
	"fmt"
	"io"
	// _ "net/http/pprof"
	"os"
)

func usage() string {
	return `simulator <src_model_file:path> <tgt_model_file:path> <trace_file:path>
this script understand the problem type implicitly`
}

func main() {
	// l, err := net.Listen("tcp", ":0")
	// if err != nil {
	// 	panic(err)
	// }
	// fmt.Printf("Listening on %s\n", l.Addr())
	// go http.Serve(l, nil)

	if len(os.Args) != 4 {
		fmt.Fprintf(os.Stderr, usage())
		os.Exit(1)
	}

	modelSrc := newModel(0)
	modelTgt := newModel(0)

	if _, err := os.Stat(os.Args[1]); err == nil {
		modelSrc, err = LoadModel(os.Args[1])
		if err != nil {
			panic(err)
		}
	}
	if _, err := os.Stat(os.Args[2]); err == nil {
		modelTgt, err = LoadModel(os.Args[2])
		if err != nil {
			panic(err)
		}
	}

	if modelSrc.resolution == 0 {
		modelSrc = newModel(modelTgt.resolution)
	}
	if modelTgt.resolution == 0 {
		modelTgt = newModel(modelSrc.resolution)
	}

	// read trace
	traceFile, err := os.Open(os.Args[3])
	if err != nil {
		panic(err)
	}
	traceReader := bufio.NewReaderSize(traceFile, 1000000)

	state := newState(modelSrc)
	commands := 0
	for {
		commands += len(state.bots)

		// fmt.Fprintf(os.Stderr, "t=%d energy=%d\n", commands, state.energy)

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

	for i := 0; i < modelTgt.resolution; i++ {
		for j := 0; j < modelTgt.resolution; j++ {
			for k := 0; k < modelTgt.resolution; k++ {
				if modelTgt.matrix[i][j][k] != state.model.matrix[i][j][k] {
					panic(fmt.Errorf("model mismatch at (%d,%d,%d) (expected = %v, but got %v)",
						k, j, i, modelTgt.matrix[i][j][k], state.model.matrix[i][j][k]))
				}
			}
		}
	}

	fmt.Printf("%d\n", state.energy)
}
