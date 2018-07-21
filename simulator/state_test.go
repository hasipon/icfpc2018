package main

import "testing"

func TestDfsOk(t *testing.T) {
	s := newState(2)

	s.model.matrix[0][0][0] = true
	s.model.matrix[0][0][1] = true
	s.model.matrix[0][1][0] = true

	s.newlyAddedPoints[Point{0, 1, 0}] = struct{}{}

	seen = make(map[Point]struct{})

	if !s.dfs(Point{0, 1, 0}) {
		t.Fail()
	}
}

func TestDfsFail(t *testing.T) {
	s := newState(2)

	s.model.matrix[0][0][0] = true
	s.model.matrix[0][0][1] = true
	s.model.matrix[1][1][0] = true

	s.newlyAddedPoints[Point{0, 1, 1}] = struct{}{}

	seen = make(map[Point]struct{})

	if s.dfs(Point{0, 1, 1}) {
		t.Fail()
	}
}

func TestCheckGroundedOk(t *testing.T) {
	s := newState(2)

	s.model.matrix[0][0][0] = true
	s.model.matrix[0][0][1] = true
	s.model.matrix[0][1][0] = true
	s.model.matrix[1][1][0] = true

	s.newlyAddedPoints[Point{0, 1, 0}] = struct{}{}
	s.newlyAddedPoints[Point{0, 1, 1}] = struct{}{}

	if !s.checkGrounded() {
		t.Fail()
	}
}

func TestCheckGroundedFail(t *testing.T) {
	s := newState(2)

	s.model.matrix[0][0][1] = true
	s.model.matrix[0][1][0] = true
	s.model.matrix[1][1][0] = true

	s.newlyAddedPoints[Point{0, 1, 0}] = struct{}{}
	s.newlyAddedPoints[Point{0, 1, 1}] = struct{}{}

	if s.checkGrounded() {
		t.Fail()
	}
}
