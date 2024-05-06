var storage = {
  file: null
};

function code_to_html(code, tag) {
  var styles = tag.includes("dstTable") ? 'style="margin-left: 30px;"' : "";
  var res = `<table class="table" id="` + tag + `" ` + styles + `>`;

  var numLine = 1;
  code.forEach((line) => {
    res += `<tr>`;
    res += `<td width="5% !important">` + numLine + "</td>";
    res += `<td width="95% !important"><code style="overflow:auto; width:100px !important">` + line + `</code></td></tr>`;
    numLine += 1;
  });
  res += `</table>`;

  return res;
}

function render_light(el, src_lines, dst_lines, numTable) {
  var type = el.checked ? "on" : "off";
  src_lines.forEach((lineNumber) => {
    var line = $("table#srcTable" + numTable)[0].getElementsByTagName("tr")[
      lineNumber - 1
    ];
    line.style.backgroundColor =
      type == "on" ? "rgba(219, 0, 0, 0.5)" : "transparent";
  });

  dst_lines.forEach((linesList) => {
    linesList.forEach((lineNumber) => {
      var line = $("table#dstTable" + numTable)[0].getElementsByTagName("tr")[
        lineNumber - 1
      ];
      line.style.backgroundColor =
        type == "on" ? "rgba(219, 0, 0, 0.5)" : "transparent";
    });
  });
}

function gen_radios(values, numTable) {
  var res = ``;
  var i = 0;
  values.forEach((v) => {
    res +=
      `
        <div class="form-check form-switch" style="display: inline-flex">
            <input class="form-check-input" type="checkbox" id="flexSwitchCheck` +
      i +
      `" onclick="render_light(this, ` +
      JSON.stringify(v[0]) +
      `, ` +
      JSON.stringify(v[1]) +
      `, ` +
      numTable +
      `)">
            <label class="form-check-label" for="flexSwitchCheck` +
      i +
      `">` +
      v[0][0] +
      ` - ` +
      v[0][v[0].length - 1] +
      `</label>
        </div>
        `;
    i += 1;
  });

  return res;
}

function saveFile() {
    var formData = new FormData();
    formData.append("file", storage.file);

    var status = $("#status");
    status.html("Загружается...");

    $.ajax({
      type: "POST",
      url: location.protocol + "//" + location.host + "/api/add_file",
      cache: false,
      contentType: false,
      processData: false,
      data: formData,
      dataType: "json",
      success: function (msg) {
        if ("message" in msg) {
          toastr.success(msg.message);
        } else {
            toastr.error("Что-то пошло не так...");
        }
      },
    });
}

function diff() {
  const subject = $("select[name=subject]").val();
  const work_type = $("select[name=work_type]").val();
  const task_num = $("select[name=task_num]").val();

  const src_filename = subject != "" ? (subject + (work_type != "" ? "_" + work_type : "") + (task_num != "" ? "_" + task_num: "")) : "all";

  var formData = new FormData();
  const file = $("#formFile")[0].files[0];
  storage.file = file;

  formData.append("file", file);
  formData.append("src_filename", src_filename);

  $.ajax({
    type: "POST",
    url: location.protocol + "//" + location.host + "/api/papa",
    cache: false,
    contentType: false,
    processData: false,
    data: formData,
    dataType: "json",
    success: function (msg) {
      $("#formFile")[0].value = "";

      // Open results if not opened already
      var btnRes = $("button[aria-controls=collapseThree]");
      if (btnRes.attr("class").includes("collapsed")) {
        btnRes.click();
      }

      if ("message" in msg) {
        $("#results").html(msg.message);
      } else {
        var resViews = [];
        for (let i of Array(msg.diff.length).keys()) {
          var tmp = {
            diff: msg.diff[i],
            src_code: msg.src_code,
            dst_code: msg.dst_code[i],
            dst_name: msg.dst_name[i],
          };

          var resultView =
            `
                <div class="container">
                    <div class="row">
                        <div class="col-12" style="width: auto">
                        ` +
            gen_radios(tmp.diff, i) +
            `
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-6">
                            <span>` +
            file.name +
            `</span>
                        ` +
            code_to_html(tmp.src_code, "srcTable" + i) +
            `
                        </div>
                        <div class="col-6">
                            <span>` +
            tmp.dst_name +
            `</span>
                        ` +
            code_to_html(tmp.dst_code, "dstTable" + i) +
            `
                        </div>
                    </div>
                </div>`;

          resViews.push(resultView);
        }

        render_data({
          fileNames: msg.dst_name,
          views: resViews,
          sims: msg.sims,
        });
      }
    },
  });
}

function render_data(d) {
  res = `
  <button type="button" class="btn btn-outline-secondary" onclick="saveFile()">Сохранить файл в базу</button>
    <ul class="nav nav-pills mb-3" id="pills-tab" role="tablist">`;
  for (let i of Array(d.fileNames.length).keys()) {
    res +=
      `<li class="nav-item" role="presentation">
    <button class="nav-link" id="pills-` +
      i +
      `-tab" data-bs-toggle="pill" data-bs-target="#pills-` +
      i +
      `" type="button" role="tab" aria-controls="pills-` +
      i +
      `" aria-selected="true">` +
      d.fileNames[i] +
      `<span class="badge bg-secondary">` + d.sims[i] + `%</span>` + 
      `</button>
    </li>
    `;
  }

  res += ` 
    </ul>
    <div class="tab-content" id="pills-tabContent">`;
  for (let i of Array(d.fileNames.length).keys()) {
    res +=
      ` <div class="tab-pane fade" id="pills-` +
      i +
      `" role="tabpanel" aria-labelledby="pills-` +
      i +
      `-tab">` +
      d.views[i] +
      `</div>`;
  }
  res += `</div>`;

  $("#results").html(res);
}


$("select[name=subject]").change(() => {
    var t = $("select[name=subject]").val()
    if (t != "") {
        $("select[name=work_type]")[0].disabled=false;
    } else {
        $("select[name=work_type]")[0].disabled=true;
        $("select[name=work_type]").val("")

        $("select[name=task_num]")[0].disabled=true;
        $("select[name=task_num]").val("")
    }
})


$("select[name=work_type]").change(() => {
    var t = $("select[name=work_type]").val()
    if (t != "") {
        $("select[name=task_num]")[0].disabled=false;
    } else {
        $("select[name=task_num]")[0].disabled=true;
        $("select[name=task_num]").val("")
    }
})