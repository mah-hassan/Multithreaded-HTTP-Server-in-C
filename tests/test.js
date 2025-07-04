import http from 'k6/http';
import { check } from 'k6';

export const options = {
  stages: [
    { duration: '10s', target: 10 },   // Light load
    { duration: '10s', target: 50 },   // Moderate
    { duration: '10s', target: 100 },  // High load (start seeing performance gap)
    { duration: '10s', target: 150 },  // Peak
    { duration: '10s', target: 200 },  // Stress
    { duration: '10s', target: 0 },    // Cooldown
  ],
};

export default function () {
  const res = http.get('http://localhost:3000/');
  check(res, {
    'status is 200': (r) => r.status === 200,
  });
}