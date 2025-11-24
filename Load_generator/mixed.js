import http from 'k6/http';
import { check } from 'k6';

export let options = {
    vus: __ENV.VUS,
    duration: '20s',
};

export default function () {
    let r = Math.random();

    if (r < 0) {
        let id = Math.floor(Math.random() * 999999);
        let res = http.get(`http://localhost:8080/get?id=${id}`);
        check(res, {
            'status ok/404': (r) => r.status === 200 || r.status === 404,
        });
    } else {
        let id = Math.floor(Math.random() * 999999);
        let res = http.post(`http://localhost:8080/set?id=${id}`, "hello");
        check(res, {
            'set ok': (r) => r.status === 200,
        });
    }
}
