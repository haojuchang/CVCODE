all:
	wget http://iccad-contest.org/2019/Problem_A/sample.tar.gz
	wget http://iccad-contest.org/2019/Problem_A/final_cases_publish_1009.tar.gz
	tar zxvf sample.tar.gz
	tar zxvf final_cases_publish_1009.tar.gz
	rm sample.tar.gz
	rm final_cases_publish_1009.tar.gz
	mv final_cases_publish_1009/* .
	rm -rf final_cases_publish_1009
	mv sample case0

clean:
	rm -rf case*