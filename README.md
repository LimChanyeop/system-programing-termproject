<h3>System programing Term project</h3>

Project Name : 식사 메뉴추천 프로그램

How to compile : code.c file을 gcc 리눅스 컴파일러를 통해 컴파일 (thread 참조 오류시 -lpthread 옵션 추가)

<h3>프로젝트 개요</h3>
식사 메뉴를 정할 때의 고민을 하거나 어려움을 겪는 경우가 많아, 유저의 선호를 반영하여 메뉴를 랜덤으로 추천해주면
식사 메뉴를 조금 더 쉽게 고를 수 있지 않을까 하는 생각에서 착안하였다.

(하지만 실제 프로그램을 활용해 본 결과.. 원하는 메뉴가 나올 때까지 계속해서 돌리거나 결국 자신이 원하는 것을 먹는 경우가 99%였음)


<h3>사용 방법 </h3>

사용자 등록 -> 메뉴 추천 (1) 메뉴 등록 (2) 선택후 안내에 맞게 입력

선택 여부에 따라 사용자의 probability.txt 파일 내부 확률 값 변경 -> 이후 실행부터 선호도 반영 

메뉴 등록시 사용자 이름으로 된 폴더 내부의 *.txt 내부에 메뉴가 입력


<h3>.txt 파일 구성 

분류별 메뉴 파일 

메뉴의 수,메뉴1,메뉴2,메뉴3, ... ,(마지막에 ','가 붙어있어야 함 -직접 메뉴 편집시 참고) 

확률 파일 (probability.txt)

한식 일식 중식 양식 순으로 숫자값 설정 (9 < x < 100) 