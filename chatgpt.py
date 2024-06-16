from openai import OpenAI

client = OpenAI(
    # defaults to os.environ.get("OPENAI_API_KEY")
    api_key = input("Please enter your API key: "),
    base_url="https://api.chatanywhere.tech/v1"
)
# 非流式响应
def gpt_35_api(messages: list):
    
    completion = client.chat.completions.create(model="gpt-3.5-turbo", messages=messages)
    print(completion.choices[0].message.content)

def gpt_35_api_stream(messages: list):
    
    stream = client.chat.completions.create(
        model='gpt-3.5-turbo',
        messages=messages,
        stream=True,
    )
    file = open("message.txt", "w")
    for chunk in stream:
        if chunk.choices[0].delta.content is not None:
            # 將messages輸出message.txt
            file.write(chunk.choices[0].delta.content)
            print(chunk.choices[0].delta.content, end="")
    file.close()
if __name__ == '__main__':
    # 讀取input.txt
    file = open("input.txt", "r")
    input_text = file.read()
    file.close()
    
    messages = [{'role': 'user','content': input_text},]
    
    gpt_35_api_stream(messages)