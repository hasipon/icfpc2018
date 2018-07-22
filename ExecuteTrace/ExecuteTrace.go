package main

import (
	"log"
	"github.com/sclevine/agouti"
	"flag"
	"os"
	"time"
	"fmt"
	"strings"
)
func Exists(filename string) bool {
	_, err := os.Stat(filename)
	return err == nil
}

func main() {
	flag.Parse()

	 src_file := flag.Arg(0)
	tgt_file := flag.Arg(1)
	trace_file := flag.Arg(2)

	if flag.NArg() != 3{
		log.Fatal("Usage: ./ExecuteTrace {src_file} {tgt_file} {trace_file}")
	}

	url := "http://18.179.226.203/ExecuteTrace/official.html"

	driver := agouti.ChromeDriver(
		agouti.ChromeOptions("args", []string{"--headless", "--disable-gpu"}),

	)
	err := driver.Start()
	if err != nil {
		log.Printf("Failed to start driver: %v", err)
	}
	defer driver.Stop()

	page, err := driver.NewPage()
	if err != nil {
		log.Printf("Failed to open page: %v", err)
	}

	err = page.Navigate(url)
	if err != nil {
		log.Printf("Failed to navigate: %v", err)
	}

	src_file_form := page.FindByID("srcModelFileIn")
	src_file_form_empty := page.FindByID("srcModelEmpty")
	tgt_file_form := page.FindByID("tgtModelFileIn")
	tgt_file_form_empty := page.FindByID("tgtModelEmpty")
	trace_file_form := page.FindByID("traceFileIn")

	submit_butoon := page.FindByID("execTrace")

	if !Exists(src_file) {
		src_file_form_empty.Click()
	} else {
		src_file_form.UploadFile(src_file)
	}

	if !Exists(tgt_file) {
		tgt_file_form_empty.Click()
	} else {
		tgt_file_form.UploadFile(tgt_file)
	}

	trace_file_form.UploadFile(trace_file)

	submit_butoon.Click()

	output := page.FindByID("stdout")
	for i:=0; i< 10; i++{
		text, err := output.Text()
		if err != nil {
			log.Printf("Failed to get html: %v", err)
		}
		if strings.Contains(text, "Failure") || strings.Contains(text, "Success"){
			fmt.Println(text)
			os.Exit(0)
		}
		time.Sleep(1 * time.Second)
	}
	os.Exit(1)
}
