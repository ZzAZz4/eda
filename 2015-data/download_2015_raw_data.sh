cat 2015_raw_data_urls.txt | xargs -n 1 -P 6 wget -c -P data/ 
