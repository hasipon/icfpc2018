package main

import "fmt"

type FusionClaim struct {
	b *NanoBot
	p Point
}

type FusionManager struct {
	primaryClaims   []FusionClaim
	secondaryClaims []FusionClaim
}

func NewFusionManager() *FusionManager {
	return &FusionManager{}
}

func (f *FusionManager) AddP(n *NanoBot, p Point) {
	f.primaryClaims = append(f.primaryClaims, FusionClaim{n, p})
}

func (f *FusionManager) AddS(n *NanoBot, p Point) {
	f.secondaryClaims = append(f.secondaryClaims, FusionClaim{n, p})
}

func (f *FusionManager) Exec() ([]int, error) {
	var removed []int

	if len(f.primaryClaims) != len(f.secondaryClaims) {
		return removed, fmt.Errorf("the number of fusions mismatched")
	}

	for _, primaryClaim := range f.primaryClaims {
		matched := false
		for _, secondaryClaim := range f.secondaryClaims {
			if primaryClaim.p == secondaryClaim.b.pos &&
				primaryClaim.b.pos == secondaryClaim.p {
				removed = append(removed, secondaryClaim.b.bid)
				primaryClaim.b.seeds = append(primaryClaim.b.seeds, secondaryClaim.b.bid)
				primaryClaim.b.seeds = append(primaryClaim.b.seeds, secondaryClaim.b.seeds...)
				matched = true
				break
			}
		}

		if !matched {
			return removed, fmt.Errorf("fusion mismatched: bid=%d claimed primary fusion but secondary bot not found", primaryClaim.b.bid)
		}
	}

	if len(removed) != len(f.secondaryClaims) {
		return removed, fmt.Errorf("fusion mismatched")
	}

	return removed, nil
}
