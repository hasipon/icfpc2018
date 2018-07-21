package main

import (
	"errors"
	"fmt"
	"sort"
	"strconv"
	"strings"
)

type Harmonics int
type Energy int64

const (
	Low Harmonics = iota
	High
)

type NanoBot struct {
	bid   int
	pos   Point
	seeds []int
}

type State struct {
	bots             []NanoBot
	energy           Energy
	harmonics        Harmonics
	model            Model
	newlyAddedPoints map[Point]struct{}
}

func newState(r int) *State {
	return &State{
		bots: []NanoBot{
			NanoBot{
				bid:   1,
				pos:   Point{x: 0, y: 0, z: 0},
				seeds: []int{2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},
			},
		},
		energy:           0,
		harmonics:        Low,
		model:            newModel(r),
		newlyAddedPoints: make(map[Point]struct{}),
	}
}

var (
	dirs = []Point{
		Point{1, 0, 0},
		Point{-1, 0, 0},
		Point{0, 1, 0},
		Point{0, -1, 0},
		Point{0, 0, 1},
		Point{0, 0, -1},
	}
)

var seen map[Point]struct{}

func (s *State) dfs(p Point) bool {
	seen[p] = struct{}{}

	if p.y == 0 {
		return true
	}

	_, ok := s.newlyAddedPoints[p]
	if !ok {
		return true
	}

	for _, dir := range dirs {
		target := p
		target.Add(dir)

		if !target.Inside(s.model.resolution) {
			continue
		}

		_, ok := seen[target]
		if ok {
			continue
		}

		if !s.model.matrix[target.z][target.y][target.x] {
			continue
		}

		if s.dfs(target) {
			return true
		}
	}

	return false
}

func (s *State) checkGrounded() bool {
	for newlyAddedPoint, _ := range s.newlyAddedPoints {
		seen = make(map[Point]struct{})

		if !s.dfs(newlyAddedPoint) {
			return false
		}
	}

	return true
}

func (s *State) update(lines []string) error {
	if s.harmonics == Low {
		if !s.checkGrounded() {
			return fmt.Errorf("not grounded")
		}
	}

	s.newlyAddedPoints = make(map[Point]struct{})

	volatiles := NewVolatiles()

	if s.harmonics == Low {
		s.energy += Energy(3 * s.model.resolution * s.model.resolution * s.model.resolution)
	} else {
		s.energy += Energy(30 * s.model.resolution * s.model.resolution * s.model.resolution)
	}

	s.energy += Energy(20 * len(s.bots))

	newBots := make([]NanoBot, 0)

	// initial volatiles
	for _, bot := range s.bots {
		if err := volatiles.Add(bot.pos); err != nil {
			return err
		}
		if !bot.pos.Inside(s.model.resolution) {
			return fmt.Errorf("out of matrix")
		}
	}

	fusionP := -1
	fusionS := -1
	removed := -1

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
			if err != nil || !lld.ValidateLld() {
				return fmt.Errorf("invalid lld %s", command[1])
			}

			unit := lld.Unit()
			length := lld.ManhattanLength()
			for j := 0; j < length; j++ {
				s.bots[i].pos.Add(*unit)
				if err = volatiles.Add(s.bots[i].pos); err != nil {
					return err
				}
				if !s.bots[i].pos.Inside(s.model.resolution) {
					return fmt.Errorf("out of matrix")
				}
			}

			s.energy += Energy(2 * length)

		case "LMove":
			sld1, err := parsePoint(command[1])
			if err != nil || !sld1.ValidateSld() {
				return fmt.Errorf("invalid sld %s", command[1])
			}
			sld2, err := parsePoint(command[2])
			if err != nil || !sld2.ValidateSld() {
				return fmt.Errorf("invalid sld %s", command[1])
			}

			unit1 := sld1.Unit()
			length1 := sld1.ManhattanLength()
			for j := 0; j < length1; j++ {
				s.bots[i].pos.Add(*unit1)
				if err = volatiles.Add(s.bots[i].pos); err != nil {
					return err
				}
				if !s.bots[i].pos.Inside(s.model.resolution) {
					return fmt.Errorf("out of matrix")
				}
			}

			unit2 := sld2.Unit()
			length2 := sld2.ManhattanLength()
			for j := 0; j < length2; j++ {
				s.bots[i].pos.Add(*unit2)
				if err = volatiles.Add(s.bots[i].pos); err != nil {
					return err
				}
				if !s.bots[i].pos.Inside(s.model.resolution) {
					return fmt.Errorf("out of matrix")
				}
			}

			s.energy += Energy(2 * (length1 + 2 + length2))

		case "FusionP":
			nd, err := parsePoint(command[1])
			if err != nil || !nd.ValidateNd() {
				return fmt.Errorf("invalid nd %s", command[1])
			}

			// TODO: Check position
			if fusionS != -1 {
				s.bots[i].seeds = append(s.bots[i].seeds, s.bots[fusionS].bid)
				s.bots[i].seeds = append(s.bots[i].seeds, s.bots[fusionS].seeds...)

				removed = fusionS

				s.energy -= 24
				fusionS = -1
			} else {
				fusionP = i
			}

		case "FusionS":
			nd, err := parsePoint(command[1])
			if err != nil || !nd.ValidateNd() {
				return fmt.Errorf("invalid nd %s", command[1])
			}

			// TODO: Check position
			if fusionP != -1 {
				s.bots[fusionP].seeds = append(s.bots[fusionP].seeds, s.bots[i].bid)
				s.bots[fusionP].seeds = append(s.bots[fusionP].seeds, s.bots[i].seeds...)

				removed = i

				s.energy -= 24
				fusionP = -1
			} else {
				fusionS = i
			}

		case "Fission":
			nd, err := parsePoint(command[1])
			if err != nil || !nd.ValidateNd() {
				return fmt.Errorf("invalid nd %s", command[1])
			}

			m, err := strconv.Atoi(command[2])
			if err != nil {
				return fmt.Errorf("invalid m %s", command[2])
			}

			if len(s.bots[i].seeds) == 0 {
				return fmt.Errorf("bot#%d is unable to fission", i)
			}
			if len(s.bots[i].seeds) < m+1 {
				return fmt.Errorf("%d is too big for the capacity of bot#%d", m, i)
			}

			target := s.bots[i].pos
			target.Add(*nd)
			if err = volatiles.Add(target); err != nil {
				return err
			}

			newBot := NanoBot{
				bid:   s.bots[i].seeds[0],
				pos:   target,
				seeds: s.bots[i].seeds[1:m],
			}
			newBots = append(newBots, newBot)

			s.energy += 24

		case "Fill":
			nd, err := parsePoint(command[1])
			if err != nil || !nd.ValidateNd() {
				return fmt.Errorf("invalid nd %s", command[1])
			}

			target := s.bots[i].pos
			target.Add(*nd)

			if s.model.matrix[target.x][target.y][target.z] {
				s.energy += 6
			} else {
				s.energy += 12
				s.model.matrix[target.x][target.y][target.z] = true
				s.newlyAddedPoints[target] = struct{}{}
			}

		default:
			return errors.New("invalid command: " + command[0])
		}
	}

	if fusionS != -1 || fusionP != -1 {
		return fmt.Errorf("fusion is missing one or more partners")
	}

	s.bots = append(s.bots, newBots...)

	if removed != -1 {
		s.bots = append(s.bots[:removed], s.bots[removed+1:]...)
	}

	sort.Slice(s.bots, func(i, j int) bool {
		return s.bots[i].bid < s.bots[j].bid
	})

	return nil
}
