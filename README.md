# 주제

Block device driver data compression

# 구현 계획

Block device에 데이터를 저장할 때, 압축해서 저장하고, 파일을 다시 읽어올 때 압축을 풀어서 사용자에게 제공해줄 수 있는 block device driver를 구현할 예정입니다.

압축 알고리즘 - 미정

추가로 사용자가 일부 파일만을 읽으려 할 때, 전체 압축을 해제하지 않도록 파일을 일정 크기 단위로 압축해 저장할 계획입니다. 이렇게 하면 사용자가 파일의 일부만을 읽으려 할 때 전체 파일의 압축을 해제하지 않고, 사용자가 읽으려는 부분의 범위만 압축을 해제하여 보여줄 수 있어서 computationally efficient 합니다.

# 성능 평가 계획 - 벤치마크

- 서로 다른 txt 파일 10000개를 테스트해, compression과 decompression이 정상적으로 동작하는지 평가
- 특정 txt 파일의 compression 된 파일을 복사했을 때, 그 복사본의 decompression이 정상적으로 동작하는지 평가
- 특정 txt 파일의 compression 된 파일을 복사하고 원본 파일을 삭제했을 때, 복사본의 decompression이 정상적으로 동작하는지 평가
- 특정 txt 파일의 일부분만 읽어오려고 할 때, 정상적으로 읽어오는지 평가 (압축 시 일정 크기 단위로 압축해서 저장할 예정)
