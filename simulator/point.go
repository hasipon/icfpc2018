package main

import (
	"fmt"
	"strconv"
	"strings"
)

type Point struct {
	x int
	y int
	z int
}

var (
	nds = []Point{
		Point{-1, -1, 0},
		Point{-1, 0, -1},
		Point{-1, 0, 0},
		Point{-1, 0, 1},
		Point{-1, 1, 0},
		Point{0, -1, -1},
		Point{0, -1, 0},
		Point{0, -1, 1},
		Point{0, 0, -1},
		Point{0, 0, 1},
		Point{0, 1, -1},
		Point{0, 1, 0},
		Point{0, 1, 1},
		Point{1, -1, 0},
		Point{1, 0, -1},
		Point{1, 0, 0},
		Point{1, 0, 1},
		Point{1, 1, 0},
	}
)

func (p Point) ManhattanLength() int {
	return AbsInt(p.x) + AbsInt(p.y) + AbsInt(p.z)
}

func (p Point) ChessboardLength() int {
	return MaxInt(MaxInt(AbsInt(p.x), AbsInt(p.y)), AbsInt(p.z))
}

func (p Point) ValidateLinear() bool {
	return (p.x == 0 && p.y == 0 && p.z != 0) || (p.x == 0 && p.y != 0 && p.z == 0) || (p.x != 0 && p.y == 0 && p.z == 0)
}

func (p Point) ValidateLld() bool {
	return p.ValidateLinear() && p.ManhattanLength() <= 15
}

func (p Point) ValidateSld() bool {
	return p.ValidateLinear() && p.ManhattanLength() <= 5
}

func (p Point) ValidateNd() bool {
	for _, nd := range nds {
		if p == nd {
			return true
		}
	}

	return false
}

func (p Point) ValidateFd() bool {
	return 0 < p.ChessboardLength() && p.ChessboardLength() <= 30
}

func (p Point) Unit() *Point {
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

func (p Point) Inside(r int) bool {
	return 0 <= p.x && p.x < r && 0 <= p.y && p.y < r && 0 <= p.z && p.z < r
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

type Volatiles struct {
	model  *Model
	points map[Point]struct{}
}

func NewVolatiles(model *Model) Volatiles {

	return Volatiles{
		model:  model,
		points: make(map[Point]struct{}),
	}
}

func (v Volatiles) Add(p Point) error {
	if v.model.matrix[p.z][p.y][p.x] {
		return fmt.Errorf("already occupied position: %v", p)
	}

	_, found := v.points[p]
	if found {
		return fmt.Errorf("already occupied position: %v", p)
	}

	v.points[p] = struct{}{}

	return nil
}
