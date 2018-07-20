package main

import (
	"bufio"
	"errors"
	"fmt"
	"io"
	"os"
	"strconv"
	"strings"
)

func usage() string {
	return `simulator <resolution:int> <model_file:path> <trace_file:path>
`
}

func AbsInt(x int) int {
	if x < 0 {
		return -x
	} else {
		return x
	}
}

func MaxInt(x, y int) int {
	if x > y {
		return x
	} else {
		return y
	}
}

type Harmonics int
type Energy int64

const (
	Low Harmonics = iota
	High
)

type Point struct {
	x int
	y int
	z int
}

func (p Point) ManhattanLength() int {
	return AbsInt(p.x) + AbsInt(p.y) + AbsInt(p.z)
}

func (p Point) LinearUnit() *Point {
	unit := &Point{}
	if p.x != 0 {
		unit.x = AbsInt(p.x) / p.x
	}
	if p.y != 0 {
		unit.y = AbsInt(p.y) / p.y
	}
	if p.z != 0 {
		unit.z = AbsInt(p.z) / p.z
	}
	return unit
}

func (p *Point) Add(q Point) {
	p.x += q.x
	p.y += q.y
	p.z += q.z
}

// <x, y, z>
// ignore spaces
func parsePoint(s string) (*Point, error) {
	p := strings.Split(strings.Replace(s[1:len(s)-1], " ", "", -1), ",")

	x, err := strconv.Atoi(p[0])
	if err != nil {
		return &Point{}, err
	}

	y, err := strconv.Atoi(p[1])
	if err != nil {
		return &Point{}, err
	}

	z, err := strconv.Atoi(p[2])
	if err != nil {
		return &Point{}, err
	}

	return &Point{
		x: x,
		y: y,
		z: z,
	}, nil
}

type NanoBot struct {
	bid      int
	pos      Point
	capacity int
}

type State struct {
	bots      []NanoBot
	energy    Energy
	harmonics Harmonics
	matrix    map[Point]struct{}
}

func newState() *State {
	return &State{
		bots: []NanoBot{
			NanoBot{
				bid:      1,
				pos:      Point{x: 0, y: 0, z: 0},
				capacity: 19,
			},
		},
		energy:    0,
		harmonics: Low,
		matrix:    make(map[Point]struct{}),
	}
}

func (s *State) update(r int, lines []string) error {
	volatiles := make(map[Point]struct{})

	if s.harmonics == Low {
		s.energy += Energy(3 * r * r * r)
	} else {
		s.energy += Energy(30 * r * r * r)
	}

	s.energy += Energy(20 * len(s.bots))

	for i, line := range lines {
		command := strings.Split(line, " ")

		switch command[0] {
		case "Halt":
			if len(s.bots) != 1 {
				return errors.New("halt: invalid number of bots")
			}
			if s.harmonics != Low {
				return errors.New("halt: harmonics must be low")
			}
			s.bots = make([]NanoBot, 0)

		case "Wait":

		case "Flip":
			if s.harmonics == Low {
				s.harmonics = High
			} else {
				s.harmonics = Low
			}

		case "SMove":
			lld, err := parsePoint(command[1])
			if err != nil {
				return fmt.Errorf("SMove: invalid lld %s", command[1])
			}

			unit := lld.LinearUnit()
			length := lld.ManhattanLength()

			for j := 0; j < length; j++ {
				_, ok := volatiles[s.bots[i].pos]
				if ok {
					return fmt.Errorf("SMove: already occupied position %s", command[1])
				}
				volatiles[s.bots[i].pos] = struct{}{}
				s.bots[i].pos.Add(*unit)
			}

			s.energy += Energy(2 * length)

		case "LMove":
		case "FusionP":
		case "FusionS":
		case "Fission":
		case "Fill":
			nd, err := parsePoint(command[1])
			if err != nil {
				return fmt.Errorf("SMove: invalid nd %s", command[1])
			}
			//TODO: validate nd

			target := s.bots[i].pos
			target.Add(*nd)

			_, found := s.matrix[target]
			if found {
				s.energy += 6
			} else {
				s.energy += 12
				s.matrix[target] = struct{}{}
			}

		default:
			return errors.New("invalid command: " + command[0])
		}
	}

	return nil
}

func main() {
	if len(os.Args) != 4 {
		fmt.Fprintf(os.Stderr, usage())
		os.Exit(1)
	}

	resolution, err := strconv.Atoi(os.Args[1])
	if err != nil {
		panic(err)
	}

	modelFile, err := os.Open(os.Args[2])
	if err != nil {
		panic(err)
	}
	bufio.NewReader(modelFile)

	traceFile, err := os.Open(os.Args[3])
	if err != nil {
		panic(err)
	}
	traceReader := bufio.NewReader(traceFile)

	state := newState()

	commands := 0

	for {
		if commands > 2000 {
			break
		}

		commands += 1

		// fmt.Printf("command: %d, %v", commands, state.bots)

		lines := make([]string, 0)

		for i := 0; i < len(state.bots); i++ {
			line, _, err := traceReader.ReadLine()
			if err == io.EOF {
				break
			}

			lines = append(lines, string(line))
		}

		err = state.update(resolution, lines)
		if err != nil {
			panic(err)
		}

		// fmt.Printf("energy = %d\n", state.energy)

		if len(state.bots) == 0 {
			break
		}
	}

	fmt.Printf("energy = %d\n", state.energy)
}
