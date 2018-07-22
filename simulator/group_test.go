package main

// func TestGroupManager1(t *testing.T) {
//     g = NewGroupManager()

//     points := []Point{
//         Point{1,1,1},
//         Point{10,1,1},
//     }

//     g.AddCommand(Region{})
// }

// func TestGroupManager3(t *testing.T) {
// 	g := NewGroupManager()

// 	points := []Point{
// 		Point{1, 1, 1}, // 0
// 		Point{2, 1, 1}, // 1
// 		Point{2, 1, 2}, // 2
// 		Point{1, 1, 2}, // 3
// 		Point{1, 2, 1}, // 4
// 		Point{2, 2, 1}, // 5
// 		Point{2, 2, 2}, // 6
// 		Point{1, 2, 2}, // 7
// 	}

// 	g.AddCommand(Region{points[0], points[6]})
// 	g.AddCommand(Region{points[6], points[0]})
// 	g.AddCommand(Region{points[1], points[7]})
// 	g.AddCommand(Region{points[7], points[1]})

// 	g.AddCommand(Region{points[2], points[4]})
// 	g.AddCommand(Region{points[4], points[2]})
// 	g.AddCommand(Region{points[3], points[5]})
// 	g.AddCommand(Region{points[5], points[3]})

// 	if !g.Valid() {
// 		t.Fail()
// 	}

// 	regions := g.Regions()

// 	if len(regions) != 1 {
// 		t.Fail()
// 	}

// 	region := regions[0]

// 	if region.src != points[0] {
// 		t.Fail()
// 	}

// 	if region.tgt != points[6] {
// 		t.Fail()
// 	}
// }
