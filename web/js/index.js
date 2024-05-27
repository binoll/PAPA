document.getElementById('subjectSelect').addEventListener('change', function () {
    var workTypeSelect = document.getElementById('workTypeSelect');
    var taskNumSelect = document.getElementById('taskNumSelect');

    if (this.value) {
        workTypeSelect.disabled = false;
    } else {
        workTypeSelect.disabled = true;
        workTypeSelect.value = "";
        taskNumSelect.disabled = true;
        taskNumSelect.value = "";
    }
});

document.getElementById('workTypeSelect').addEventListener('change', function () {
    var taskNumSelect = document.getElementById('taskNumSelect');

    if (this.value) {
        taskNumSelect.disabled = false;
    } else {
        taskNumSelect.disabled = true;
        taskNumSelect.value = "";
    }
});
