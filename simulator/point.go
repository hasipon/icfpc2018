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

func validateNd(p Point) error {
	for _, nd := range nds {
		if p == nd {
			return nil
		}
	}

	return fmt.Errorf("validateNd: %v", p)
}
