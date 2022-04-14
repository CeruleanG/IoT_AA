function doGet(e)
{
  var mo = e.parameter.func;
  if(mo == "addData")
  {
    var stat = add_data(e);
    if(stat == 1)
    {
      var result = 
      {
        status : true
      };
      return ContentService.createTextOutput(JSON.stringify(result)).setMimeType(ContentService.MimeType.JSON);
    }
  }
}

function add_data(e)
{
  // remplacez l'URL ci-dessous par l'URL de votre feuille de calcul sur Google Sheets
  var sheet = SpreadsheetApp.openByUrl('https://docs.google.com/spreadsheets/d/AKfycbws1eEV99HAfV9MSQ3hltqjnsiSrM64Hz3q4QFGVk7T3NNUXxE/edit#gid=0');

  var CurrentDate = new Date();
  var Date_ = Utilities.formatDate(CurrentDate, "America/Montreal", "dd/MM/YYYY");
  var Time_ = Utilities.formatDate(CurrentDate, "America/Montreal", "HH:mm:ss");
  
  sheet.appendRow([Date_, Time_, e.parameter.val1]);
  return 1;
}