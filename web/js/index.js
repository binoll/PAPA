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

document.addEventListener("DOMContentLoaded", function () {
    const results = document.querySelectorAll('.result-card');
    results.forEach((result, index) => {
        result.style.animationDelay = `${index * 0.1}s`;
        result.classList.add('animate__animated', 'animate__fadeInUp');
    });
});

document.querySelectorAll('.accordion-button').forEach(button => {
    button.addEventListener('click', function () {
        const currentlyActive = document.querySelector('.accordion-button:not(.collapsed)');
        if (currentlyActive && currentlyActive !== this) {
            currentlyActive.classList.add('collapsed');
            currentlyActive.nextElementSibling.classList.remove('show');
        }
    });
});

document.getElementById('formFile').addEventListener('change', function () {
    var fileLabel = document.getElementById('fileLabel');
    if (this.files && this.files.length > 0) {
        fileLabel.textContent = this.files[0].name;
        fileLabel.classList.remove('btn-info');
        fileLabel.classList.add('btn-primary');
    } else {
        fileLabel.textContent = "Загрузите ваш файл";
        fileLabel.classList.remove('btn-primary');
        fileLabel.classList.add('btn-info');
    }
});

document.querySelector('form').addEventListener('submit', function (event) {
    event.preventDefault();
    const newResult = document.createElement('div');
    newResult.classList.add('result-card', 'animate__animated', 'animate__fadeInUp');
    newResult.style.animationDelay = '0s';
    newResult.innerHTML = '<p>Новый результат</p>';
    document.getElementById('results').appendChild(newResult);
});
