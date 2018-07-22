package main

import (
	"log"
	"github.com/sclevine/agouti"
	"flag"
)

func main() {
	flag.Parse()

	/*
	 src_file := flag.Arg(0)
	tgt_file := flag.Arg(1)
	trace_file := flag.Arg(2)
	*/
	if flag.NArg() != 3{
		log.Fatal("Usage: ./ExecuteTrace {src_file} {tgt_file} {trace_file}")
	}

	url := "http://18.179.226.203/ExecuteTrace/official.html"

	driver := agouti.ChromeDriver()
	err := driver.Start()
	if err != nil {
		log.Printf("Failed to start driver: %v", err)
	}
	defer driver.Stop()

	page, err := driver.NewPage(agouti.Browser("chrome"))
	if err != nil {
		log.Printf("Failed to open page: %v", err)
	}

	err = page.Navigate(url)
	if err != nil {
		log.Printf("Failed to navigate: %v", err)
	}

	// contentにHTMLが入る
	content, err := page.HTML()
	if err != nil {
		log.Printf("Failed to get html: %v", err)
	}
	log.Println(content)


}
