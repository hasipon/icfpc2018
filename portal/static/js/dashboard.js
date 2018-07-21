var init_timezone = function() {
  moment.tz.setDefault("Asia/Tokyo");
  moment.locale('ja')
}

var update_countdown = function() {
  const datetime_format = "YYYYMMDDHHmm"
  const start_datetime= "201708052100"
  const lightning_datetime= "201708062100"
  const end_datetime= "201708082100"

  m = moment()

  var diff = moment(end_datetime, datetime_format).diff(m)
  if (0 < diff) {
    const duration = moment.duration(diff)
    var text = moment.utc(duration.as('milliseconds')).format('HH:mm:ss')
    $('#contest-timer').text("コンテスト終了まで " + text)
    return
  }

  $('#contest-timer').text("コンテスト終了")
}

$(function() {
  init_timezone()
  setInterval(update_countdown, 1000)
})
