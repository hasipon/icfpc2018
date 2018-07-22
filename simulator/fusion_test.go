package main

import (
	"testing"
)

func testEq(a, b []int) bool {
	if a == nil && b == nil {
		return true
	}

	if a == nil || b == nil {
		return false
	}

	if len(a) != len(b) {
		return false
	}

	for i := range a {
		if a[i] != b[i] {
			return false
		}
	}

	return true
}

func TestFusionManagerOk(t *testing.T) {
	f := NewFusionManager()

	n1 := NanoBot{
		bid:   1,
		pos:   Point{0, 0, 0},
		seeds: []int{2, 3, 4},
	}
	n2 := NanoBot{
		bid:   5,
		pos:   Point{1, 1, 0},
		seeds: []int{6, 7},
	}

	f.AddP(&n1, Point{1, 1, 0})
	f.AddS(&n2, Point{0, 0, 0})

	bids, err := f.Exec()
	if err != nil {
		t.Fatalf("err = %v", err)
	}

	if !testEq(bids, []int{5}) {
		t.Errorf("bids wrong: %v", bids)
	}

	if !testEq(n1.seeds, []int{2, 3, 4, 5, 6, 7}) {
		t.Errorf("seeds wrong: %v", n1.seeds)
	}
}

func TestFusionManagerOkDouble(t *testing.T) {
	f := NewFusionManager()

	n1 := NanoBot{
		bid:   1,
		pos:   Point{0, 0, 0},
		seeds: []int{2, 3, 4},
	}
	n2 := NanoBot{
		bid:   5,
		pos:   Point{1, 1, 0},
		seeds: []int{6, 7},
	}
	n3 := NanoBot{
		bid:   8,
		pos:   Point{10, 20, 30},
		seeds: []int{},
	}
	n4 := NanoBot{
		bid:   9,
		pos:   Point{10, 20, 31},
		seeds: []int{},
	}

	f.AddP(&n1, Point{1, 1, 0})
	f.AddS(&n2, Point{0, 0, 0})
	f.AddP(&n3, Point{10, 20, 31})
	f.AddS(&n4, Point{10, 20, 30})

	bids, err := f.Exec()
	if err != nil {
		t.Fatalf("err = %v", err)
	}
	if !testEq(bids, []int{5, 9}) {
		t.Errorf("bids wrong: %v", bids)
	}
	if !testEq(n1.seeds, []int{2, 3, 4, 5, 6, 7}) {
		t.Errorf("seeds wrong: %v", n1.seeds)
	}
	if !testEq(n3.seeds, []int{9}) {
		t.Errorf("seeds wrong: %v", n3.seeds)
	}
}

func TestFusionManagerFail(t *testing.T) {
	n1 := NanoBot{
		bid:   1,
		pos:   Point{0, 0, 0},
		seeds: []int{2, 3, 4},
	}
	n2 := NanoBot{
		bid:   5,
		pos:   Point{1, 1, 0},
		seeds: []int{6, 7},
	}

	f := NewFusionManager()
	f.AddP(&n1, Point{1, 1, 1})
	f.AddS(&n2, Point{0, 0, 0})
	if _, err := f.Exec(); err == nil {
		t.Fatalf("error should not be nil")
	}

	f = NewFusionManager()
	f.AddP(&n1, Point{1, 1, 0})
	f.AddS(&n2, Point{0, 0, 1})
	if _, err := f.Exec(); err == nil {
		t.Fatalf("error should not be nil")
	}

	f = NewFusionManager()
	f.AddP(&n1, Point{1, 1, 0})
	if _, err := f.Exec(); err == nil {
		t.Fatalf("error should not be nil")
	}

	f = NewFusionManager()
	f.AddS(&n2, Point{1, 1, 0})
	if _, err := f.Exec(); err == nil {
		t.Fatalf("error should not be nil")
	}
}

func TestFusionManagerFailDouble(t *testing.T) {
	n1 := NanoBot{
		bid:   1,
		pos:   Point{0, 0, 0},
		seeds: []int{2, 3, 4},
	}
	n2 := NanoBot{
		bid:   5,
		pos:   Point{1, 1, 0},
		seeds: []int{6, 7},
	}
	n3 := NanoBot{
		bid:   8,
		pos:   Point{10, 20, 30},
		seeds: []int{},
	}
	n4 := NanoBot{
		bid:   9,
		pos:   Point{10, 20, 31},
		seeds: []int{},
	}
	n5 := NanoBot{
		bid:   10,
		pos:   Point{1, 0, 1},
		seeds: []int{},
	}

	f := NewFusionManager()
	f.AddP(&n1, Point{1, 1, 0})
	f.AddS(&n2, Point{0, 0, 0})
	f.AddP(&n3, Point{10, 20, 31})
	f.AddS(&n4, Point{10, 20, 31})
	if res, err := f.Exec(); err == nil {
		t.Errorf("error should not be nil: %v", res)
	}

	f = NewFusionManager()
	f.AddP(&n1, Point{1, 1, 0})
	f.AddS(&n2, Point{0, 0, 0})
	f.AddP(&n5, Point{0, 0, 0})
	f.AddS(&n4, Point{10, 20, 31})
	if res, err := f.Exec(); err == nil {
		t.Errorf("error should not be nil: %v", res)
	}
}
