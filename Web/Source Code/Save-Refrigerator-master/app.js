// 데이터 링크
link = 'https://api.thingspeak.com/channels/575626/feeds.json?api_key=EUL7KB1QYEAQQX0S&results=2';

// 열려있는 시간
var time;
// 전기사용량, 전기요금, 탄소배출량 총합
var esum = 0, bsum = 0, csum = 0;
// 제품번호 배열
product = new Array();

$(window).ready(function(event){
    check = setInterval(function(event){
        $('#check').load(link, function(data){
            // json으로 data불러오기
            var obj = JSON.parse(data);

            // 태그, 열린 시간 불러오기
            tag = parseFloat(obj["feeds"][0]["field1"]);
            time = parseFloat(obj["feeds"][0]["field2"]);

            // 현재 시간과 열린 시간 check.html에 추가
            var date = new Date();
            var dateStr = date.getFullYear() + "년 " + date.getMonth() + "월 " + date.getDate() + "일 " + date.getHours() + "시 " + date.getMinutes() + "분 " + date.getSeconds() + "초";
            $('#section-list #time .sub-title').html(dateStr + " - " + time + "초");

            // 제품 배열 확인하고 check.html에 추가
            if(checkProduct(tag) == 1)
            {
                product.append(tag);
                $('#section-list #rfid .sub-title').html(tag + "번 제품이 냉장고 안에 들어갔습니다.");
            }
            else if(checkProduct(tag) == -1)
                $('#section-list #rfid .sub-title').html(tag + "번 제품이 냉장고 밖으로 나갔습니다.");
            else if(checkProduct(tag) == 0)
                $('#section-list #rfid .sub-title').html("냉장고 안에 아무 제품도 들어있지 않습니다.");
        });

        if(typeof(time) == 'number')
        {
            // 총합 계산
            esum = parseFloat((time * 0.027).toFixed(3));
            bsum = parseFloat((time * 0.004).toFixed(3));
            csum = parseFloat((time * 4.24).toFixed(3));
        
            str1 = esum + "W";
            str2 = bsum + "원";
            str3 = csum + "g";

            // check.html에 추가
            $('#section-list #elec .sub-title').html(str1);
            $('#section-list #Bill .sub-title').html(str2);
            $('#section-list #Cb .sub-title').html(str3);
        }

    }, 3000);

    // 그래프 삽입
    $('#temp .graph').append("<iframe width='450' height='260' style='border: 1px solid #cccccc;' src='https://thingspeak.com/channels/575240/charts/1?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15'></iframe>");
    $('#mois .graph').append("<iframe width='450' height='260' style='border: 1px solid #cccccc;' src='https://thingspeak.com/channels/575240/charts/2?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15'></iframe>");
    $('#oxy .graph').append("<iframe width='450' height='260' style='border: 1px solid #cccccc;' src='https://thingspeak.com/channels/575240/charts/3?bgcolor=%23ffffff&color=%23d62020&dynamic=true&results=60&type=line&update=15'></iframe>");
});

//상품 중복 확인
function checkProduct(value)
{
    if(product.length == 0)
        return 0;
    for(i = 0; i < product.length; i++)
    {
        if(value == product[i]) 
            return -1;
    }
    return 1;
}

// 체험하기
$('#section-check button').on("click", function(event){
    window.location = 'check.html';
});

// 한영 번역
var num = 0;
$('#lan').on("click", function(event){
    if(num%2)
    {
        $(this).html("English");
        $('.kor').show();
        $('.en').hide();
    }
    else
    {
        $(this).html("한국어");
        $('.kor').hide();
        $('.en').show();
    }
    num++;
});

// 개발방법 더 알아보기
$('#section-what p').on("click", function(event){
    window.location = 'code.html';
});